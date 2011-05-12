#ifndef HISTOGRAMPLOT_H
#define HISTOGRAMPLOT_H

class HistogramPlot
{
public:

  HistogramPlot(int numDataArray, int numBins);
  void AddPlot(double *beginDataArray);
  void Plot();

  int numDataSets;
  int addingSetNumber;
  double* dataSetStart[3];
  int numBins;
};

#endif // HISTOGRAMPLOT_H
