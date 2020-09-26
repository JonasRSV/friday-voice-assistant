#ifndef CPPFLOW_H_C0VYXYJB
#define CPPFLOW_H_C0VYXYJB

#include "tensorflow/include/tensorflow/c/c_api.h"
#include <algorithm>
#include <cstring>
#include <fstream>
#include <iostream>
#include <memory>
#include <numeric>
#include <string>
#include <tuple>
#include <vector>

class Model;
class Tensor;

class Tensor {
public:
  Tensor(const Model &model, const std::string &op_name);
  ~Tensor();

  template <typename T> void set_data(T *data, size_t size);
  template <typename T> T get_data();

  int flag;
  void clean();
  TF_Output op;
  TF_Tensor *val;

private:
  TF_DataType type;
  int n_dims;
  int64_t *dims;
  void *data;

  template <typename T> TF_DataType deduce_type();
  void error_check(bool condition, const std::string &error);
};

class Model {
public:
  // Pass a path to the model file and optional Tensorflow config options. See
  // examples/load_model/main.cpp.
  explicit Model(const std::string &export_dir);

  ~Model();

  void init();
  std::vector<std::string> get_operations() const;

  // Original Run
  void run(Tensor *input, Tensor *output);

private:
  TF_Graph *graph;
  TF_Session *session;
  TF_Status *status;

  bool status_check(bool throw_exc) const;
  void error_check(bool condition, const std::string &error) const;

public:
  friend class Tensor;
};

#endif /* end of include guard: CPPFLOW_H_C0VYXYJB */
