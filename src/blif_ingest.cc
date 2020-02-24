#include "blif_ingest.h"

#include <iostream>
#include <string>

namespace blifdot {

Edge *Graph::FindOrCreateEdge(const std::string &name) {
  Edge *net;
  auto edge_it = edges_.find(name);
  if (edge_it != edges_.end()) {
    net = edge_it->second;
  } else {
    net = new Edge({name, {}, {}});
    edges_.insert({name, net});
  }
  return net;
}

void Graph::AddVertex(
      const std::vector<std::string> &input_edge_names,
      const std::string &output_edge_name) {
  Vertex *vertex =
      new Vertex({std::to_string(vertices_.size()), {}, nullptr, 0.0});
  Edge *out_edge = FindOrCreateEdge(output_edge_name);
  out_edge->in.insert(vertex);
  vertex->out = out_edge;
  for (const auto &input : input_edge_names) {
    Edge *in_edge = FindOrCreateEdge(input);
    in_edge->out.insert(vertex);
    vertex->in.push_back(in_edge);
  }
  vertices_.push_back(vertex);
}

void Graph::AddInputEdges(const std::vector<std::string> &input_edge_names) {
  for (const std::string &input : input_edge_names) {
    Edge *net = FindOrCreateEdge(input);
    net->in.insert(new Vertex(
          {"EXTERNAL_IN_" + std::to_string(inputs_.size()), {}, nullptr, 0.0}));
    inputs_.push_back(net);
  }
}

void Graph::AddOutputEdges(const std::vector<std::string> &output_edge_names) {
  for (const std::string &output : output_edge_names) {
    Edge *net = FindOrCreateEdge(output);
    net->out.insert(new Vertex(
          {"EXTERNAL_OUT_" + std::to_string(outputs_.size()), {}, nullptr, 0.0}));
    outputs_.push_back(net);
  }
}

void Graph::Print() const {
  std::cout << "Graph \"" << name_ << "\":" << std::endl
      << "\tinputs: " << inputs_.size() << std::endl;
  for (const auto &edge : inputs_)
    std::cout << "\t\t" << edge->name << std::endl;
  std::cout << "\toutputs: " << outputs_.size() << std::endl;
  for (const auto &edge : outputs_)
    std::cout << "\t\t" << edge->name << std::endl;

  std::cout << "\tvertices: " << vertices_.size() << std::endl;
  std::cout << "\tedges: " << edges_.size() << std::endl;
}

void Graph::WriteDOT() const {
  std::cout << "digraph " << name_ << std::endl << "{" << std::endl;
  for (const auto &iter : edges_) {
    Edge *edge = iter.second;
    //std::cout << "// " << edge->name
    //          << " in: " << edge->in.size()
    //          << " out: " << edge->out.size() << std::endl;
    for (const Vertex *in : edge->in) {
      for (const Vertex *out : edge->out) {
        std::cout << "  " << in->name << " -> " << out->name
                  << " [label=\"" << edge->name << "\"]" << std::endl;
      }
    }
  }
  std::cout << "}" << std::endl;
}

void BlifIngest::start_parse() {
  // Pass.
}

void BlifIngest::filename(std::string fname) {}

void BlifIngest::lineno(int line_num) {
  line_num_ = line_num;
}

void BlifIngest::begin_model(std::string model_name) {
  current_graph_ = new Graph(model_name);
}

void BlifIngest::inputs(std::vector<std::string> inputs) {
  current_graph_->AddInputEdges(inputs);
}

void BlifIngest::outputs(std::vector<std::string> outputs) {
  current_graph_->AddOutputEdges(outputs);
}

void BlifIngest::names(std::vector<std::string> nets,
                       std::vector<std::vector<blifparse::LogicValue>> so_cover) {
  // The last named net is the output, all the preceeding ones are inputs.
  std::string output_edge_name = nets.back();
  nets.pop_back();

  current_graph_->AddVertex(nets, output_edge_name);
}

void BlifIngest::latch(std::string input,
                       std::string output,
                       blifparse::LatchType type,
                       std::string control,
                       blifparse::LogicValue init) {
  current_graph_->AddVertex({input}, output);
}

void BlifIngest::end_model() {
  graphs_.insert({current_graph_->name(), current_graph_});
  current_graph_ = nullptr;
}

void BlifIngest::subckt(std::string model,
                        std::vector<std::string> ports,
                        std::vector<std::string> nets) {
  std::cout << line_num_ << ": UNIMPLEMENTED .subckt: " << model << std::endl;
}

void BlifIngest::blackbox() {
  std::cout << line_num_ << ": UNIMPLEMENTED .blackbox" << std::endl;
}

void BlifIngest::conn(std::string src, std::string dst) {
  std::cout << line_num_ << ": UNIMPLEMENTED .conn" << std::endl;
}

void BlifIngest::cname(std::string cell_name) {
  std::cout << line_num_ << ": UNIMPLEMENTED .cname" << std::endl;
}

void BlifIngest::attr(std::string name, std::string value) {
  std::cout << line_num_ << ": UNIMPLEMENTED .attr" << std::endl;
}

void BlifIngest::param(std::string name, std::string value) {
  std::cout << line_num_ << ": UNIMPLEMENTED .param" << std::endl;
}

void BlifIngest::finish_parse() {
  for (auto &graph_it : graphs_)
    graph_it.second->WriteDOT();
}

void BlifIngest::parse_error(const int curr_lineno,
                             const std::string &near_text,
                             const std::string &msg) {
  std::cerr << curr_lineno << ": PARSE ERROR. " << msg << std::endl
            << "\t" << near_text << std::endl;
}

} // namespace blifdot
