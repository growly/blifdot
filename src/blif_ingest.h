#ifndef _BLIF_INGEST_H_
#define _BLIF_INGEST_H_

#include <set>
#include <string>
#include <vector>
#include <unordered_map>

#include "blifparse.hpp"

namespace blifdot {

struct Edge;

// This abstractly represents an vertex in the circuit.
struct Vertex {
  std::string name;
  std::vector<Edge*> in;
  Edge* out;
  double weight;
};

// This abstractly represents an edge in the circuit. It's a net.
struct Edge {
  std::string name;
  std::set<Vertex*> in;
  std::set<Vertex*> out;
};

// This abstractly represents a self-contained graph, i.e. a model.
// TODO(aryap): This owns its child objects and must delete them.
class Graph {
 public:
  Graph(const std::string &name)
      : name_(name) {}

  void AddInputEdges(const std::vector<std::string> &input_edge_names);
  void AddOutputEdges(const std::vector<std::string> &output_edge_names);

  void AddVertex(
      const std::vector<std::string> &input_edge_names,
      const std::string &output_edge_name);

  void Print() const;

  void WriteDOT() const;

  const std::string &name() const { return name_; }

 private:
  Edge *FindOrCreateEdge(const std::string &name);

  std::string name_;
  std::vector<Edge*> inputs_;
  std::vector<Edge*> outputs_;

  std::vector<Vertex*> vertices_;
  std::unordered_map<std::string, Edge*> edges_;
};

class BlifIngest : public blifparse::Callback {
 public:
  BlifIngest() {}
  virtual ~BlifIngest() override {}

  void start_parse() override;
  void filename(std::string fname) override;
  void lineno(int line_num) override;
  void begin_model(std::string model_name) override;
  void inputs(std::vector<std::string> inputs) override;
  void outputs(std::vector<std::string> outputs) override;
  void names(std::vector<std::string> nets,
             std::vector<std::vector<blifparse::LogicValue>> so_cover) override;
  void latch(std::string input, std::string output, blifparse::LatchType type,
             std::string control, blifparse::LogicValue init) override;
  void subckt(std::string model,
              std::vector<std::string> ports,
              std::vector<std::string> nets) override;
  void blackbox() override;
  void end_model() override;
  void conn(std::string src, std::string dst) override;
  void cname(std::string cell_name) override;
  void attr(std::string name, std::string value) override;
  void param(std::string name, std::string value) override;
  void finish_parse() override;
  void parse_error(const int curr_lineno,
                   const std::string &near_text,
                   const std::string &msg) override;
 
 private:
  Graph *current_graph_;

  int line_num_;

  // Maps model names to graphs of models.
  std::unordered_map<std::string, Graph*> graphs_;
};

} // namespace blifdot

#endif // _BLIF_INGEST_H_
