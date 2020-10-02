#include "tf.h"
#include <memory>
#include <utility>

Tensor::Tensor(const Model &model, const std::string &op_name) {
  // This code prepairs an operation

  // Get operation by the name
  this->op.oper = TF_GraphOperationByName(model.graph, op_name.c_str());
  this->op.index = 0;

  // Operation did not exists
  error_check(this->op.oper != nullptr,
              "No operation named \"" + op_name + "\" exists");

  // Get number of dimensions
  this->n_dims = TF_GraphGetTensorNumDims(model.graph, this->op, model.status);

  // DataType
  this->type = TF_OperationOutputType(this->op);

  // If is not a scalar
  if (this->n_dims > 0) {
    // Get dimensions
    this->dims = new int64_t[this->n_dims];
    TF_GraphGetTensorShape(model.graph, this->op, this->dims, this->n_dims,
                           model.status);

    // Check error on Model Status
    model.status_check(true);
  }

  this->flag = 0;
  this->val = nullptr;
  this->data = nullptr;
}

Tensor::~Tensor() { this->clean(); }

void Tensor::clean() {
  if (this->flag == 1) {
    TF_DeleteTensor(this->val);
    this->flag = 0;
  }
  this->data = nullptr;
}

void Tensor::error_check(bool condition, const std::string &error) {
  if (!condition) {
    throw std::runtime_error(error);
  }
}

template <typename T> void Tensor::set_data(T *data, size_t size) {

  // Non empty tensor
  if (this->flag == 1) {
    TF_DeleteTensor(this->val);
    this->flag = 0;
  }

  // Check Tensor is valid
  this->error_check(this->flag != -1, "Tensor is not valid");

  // Check type
  this->error_check(deduce_type<T>() == this->type,
                    "Provided type is different from Tensor expected type");

  // Deallocator
  auto d = [](void *ddata, size_t, void *) { free(static_cast<T *>(ddata)); };

  // Saves data on class
  this->data = malloc(sizeof(T) * size);
  memcpy(this->data, data, sizeof(T) * size);

  this->val = TF_NewTensor(/*TF_DataType=*/this->type,
                           /*dims*/ this->dims,
                           /*num_dims*/ this->n_dims,
                           /*data=*/this->data,
                           /*length=*/sizeof(T) * size,
                           /*deallocator=*/d,
                           /*deallocator_args=*/nullptr);

  this->error_check(this->val != nullptr,
                    "An error occurred allocating the Tensor memory");

  this->flag = 1;
}

template <typename T> std::vector<T> Tensor::get_data() {

  // Check Tensor is valid
  this->error_check(this->flag != -1, "Tensor is not valid");

  // Check type
  this->error_check(deduce_type<T>() == this->type,
                    "Expected return type is different from Tensor type");

  // Tensor is not empty
  this->error_check(this->flag != 0, "Tensor is empty");

  // Check tensor data is not empty
  auto raw_data = TF_TensorData(this->val);
  this->error_check(raw_data != nullptr, "Tensor data is empty");

  // Convert to correct type
  const auto T_data = static_cast<T *>(raw_data);

  return std::vector<T>(T_data, T_data + this->dims[0]);
}

template <typename T> TF_DataType Tensor::deduce_type() {
  if (std::is_same<T, float>::value)
    return TF_FLOAT;
  if (std::is_same<T, double>::value)
    return TF_DOUBLE;
  if (std::is_same<T, int32_t>::value)
    return TF_INT32;
  if (std::is_same<T, uint8_t>::value)
    return TF_UINT8;
  if (std::is_same<T, int16_t>::value)
    return TF_INT16;
  if (std::is_same<T, int8_t>::value)
    return TF_INT8;
  if (std::is_same<T, int64_t>::value)
    return TF_INT64;
  //    if constexpr (std::is_same<T, bool>::value)
  //        return TF_BOOL;
  if (std::is_same<T, uint16_t>::value)
    return TF_UINT16;
  if (std::is_same<T, uint32_t>::value)
    return TF_UINT32;
  if (std::is_same<T, uint64_t>::value)
    return TF_UINT64;

  throw std::runtime_error{"Could not deduce type!"};
}

// VALID deduce_type TEMPLATES
template TF_DataType Tensor::deduce_type<float>();
template TF_DataType Tensor::deduce_type<double>();
// template TF_DataType Tensor::deduce_type<bool>();
template TF_DataType Tensor::deduce_type<int8_t>();
template TF_DataType Tensor::deduce_type<int16_t>();
template TF_DataType Tensor::deduce_type<int32_t>();
template TF_DataType Tensor::deduce_type<int64_t>();
template TF_DataType Tensor::deduce_type<uint8_t>();
template TF_DataType Tensor::deduce_type<uint16_t>();
template TF_DataType Tensor::deduce_type<uint32_t>();
template TF_DataType Tensor::deduce_type<uint64_t>();

// VALID get_data TEMPLATES
template std::vector<float> Tensor::get_data<float>();

// VALID set_data TEMPLATES
template void Tensor::set_data<int16_t>(int16_t *new_data, size_t size);
template void Tensor::set_data<int32_t>(int32_t *new_data, size_t size);

Model::Model(const std::string &export_dir) {

  this->status = TF_NewStatus();
  this->graph = TF_NewGraph();

  // Create the session.
  TF_SessionOptions *sess_opts = TF_NewSessionOptions();
  TF_Buffer *run_opts = NULL;
  const char *tags = "serve";

  this->session = TF_LoadSessionFromSavedModel(
      /*session_options=*/sess_opts,
      /*run_options=*/run_opts,
      /*export_dir=*/export_dir.c_str(),
      /*tags=*/&tags,
      /*ntags=*/1,
      /*graph=*/this->graph, NULL,
      /*status=*/this->status);

  // Can delete sess opts here already since we wont use em
  TF_DeleteSessionOptions(sess_opts);

  this->status_check(true);
}

Model::~Model() {
  TF_DeleteSession(this->session, this->status);
  TF_DeleteGraph(this->graph);
  this->status_check(true);
  TF_DeleteStatus(this->status);
}

void Model::init() {
  TF_Operation *init_op[1] = {TF_GraphOperationByName(this->graph, "init")};

  this->error_check(init_op[0] != nullptr,
                    "Error: No operation named \"init\" exists");

  TF_SessionRun(this->session, nullptr, nullptr, nullptr, 0, nullptr, nullptr,
                0, init_op, 1, nullptr, this->status);
  this->status_check(true);
}

std::vector<std::string> Model::get_operations() const {
  std::vector<std::string> result;
  size_t pos = 0;
  TF_Operation *oper;

  // Iterate through the operations of a graph
  while ((oper = TF_GraphNextOperation(this->graph, &pos)) != nullptr) {
    result.emplace_back(TF_OperationName(oper));
  }

  return result;
}

void Model::run(Tensor *input, Tensor *output) {

  this->error_check(input->flag == 1, "Error: input is not full");
  this->error_check(output->flag != -1, "Error: output is not valid");

  // Clean previous stored output
  output->clean();

  // Get input operation
  TF_Output input_op = input->op;
  TF_Tensor *input_value = input->val;

  // Get output operation
  TF_Output output_op = output->op;

  TF_Tensor *output_value;

  TF_SessionRun(
      /*session=*/this->session,
      /*run_options=*/nullptr,
      /*inputs=*/&input_op,
      /*input_values=*/&input_value,
      /*num_inputs=*/1,
      /*outputs=*/&output_op,
      /*output_values=*/&output_value,
      /*num_outputs=*/1,
      /*target_operations=*/nullptr,
      /*num_targets=*/0,
      /*run_metadata=*/nullptr,
      /*status=*/this->status);

  this->status_check(true);

  // Save result on output and mark as full
  output->val = output_value;
  output->flag = 1;

  // Empty input
  input->clean();
}

bool Model::status_check(bool throw_exc) const {

  if (TF_GetCode(this->status) != TF_OK) {
    if (throw_exc) {
      throw std::runtime_error(TF_Message(status));
    } else {
      return false;
    }
  }
  return true;
}

void Model::error_check(bool condition, const std::string &error) const {
  if (!condition) {
    throw std::runtime_error(error);
  }
}
