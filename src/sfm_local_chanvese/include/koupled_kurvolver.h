#ifndef KOUPLED_KURVOLVER_H
#define KOUPLED_KURVOLVER_H

class KoupledKurvolver
{
public:
  struct Options {
    Options(int ac, char* av [] ) {
      // use Boost Program Options to parse and setup
    }
  };

public:
  KoupledKurvolver(const KoupledKurvolver::Options& opts_in);

  void Print();

private:
  KoupledKurvolver() { }

};

#endif // KOUPLED_KURVOLVER_H
