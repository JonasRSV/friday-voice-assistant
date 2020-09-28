#include "../../../shared/aixlog.hpp"
#include "../../../shared/json.hpp"
#include "cppflow/cppflow.h"
#include "goldfish.hpp"

namespace goldfish {
std::string config() { return "goldfish"; }

Model *model;
Tensor *input;
Tensor *output;

void setup(const nlohmann::json config) {

  if (!config.contains("export_dir")) {
    LOG(FATAL) << TAG("goldfish") << AixLog::Color::RED
               << "Config does not contain export_dir" << AixLog::Color::NONE
               << std::endl;
    exit(1);
  }

  std::string export_dir = config["export_dir"];

  LOG(INFO) << TAG("goldfish") << AixLog::Color::YELLOW
            << "Loading model from: " << export_dir << AixLog::Color::NONE
            << std::endl;

  model = new Model(export_dir);

  LOG(INFO) << TAG("goldfish") << AixLog::Color::YELLOW << "Initializing model"
            << AixLog::Color::NONE << std::endl;

  input = new Tensor(*model, "input");
  output = new Tensor(*model, "output");

  LOG(INFO) << TAG("goldfish") << AixLog::Color::GREEN
            << "Loaded model from: " << export_dir << AixLog::Color::NONE
            << std::endl;
}
model_prediction predict(int16_t *audio, size_t size) {
  LOG(DEBUG) << TAG("goldfish") << AixLog::Color::YELLOW
             << "Allocating audio of size: " << size << " on Tensor"
             << AixLog::Color::NONE << std::endl;
  input->set_data(audio, size);
  LOG(DEBUG) << TAG("goldfish") << AixLog::Color::YELLOW << "Predicting"
             << AixLog::Color::NONE << std::endl;
  model->run(input, output);

  LOG(DEBUG) << TAG("goldfish") << AixLog::Color::GREEN
             << "Predicting Successful" << AixLog::Color::NONE << std::endl;

  return model_prediction{output->get_data<float>()};
}

} // namespace goldfish
