
#ifndef __vtkInitClosedPath_h
#define __vtkInitClosedPath_h

#include "vtkPolyDataAlgorithm.h"
#include "vtkSmartPointer.h"
#include "vtkFloatArray.h"
#include "callback.h"

class vtkInitClosedPath : public vtkPolyDataAlgorithm // , public vrcl::observable
{
public:

  const static std::string CONTOUR_ARRAY_NAME;
  const static std::string DISTANCEMAP_ARRAY_NAME;

  vtkTypeRevisionMacro( vtkInitClosedPath, vtkPolyDataAlgorithm)
  void PrintSelf(ostream& os, vtkIndent indent);

  static vtkInitClosedPath *New();

  void SetSource(vtkPolyData *source);


  /** set the seed vertices via copy of input
    */
  void SetInitPointVertexIdx( vtkIntArray* initVertIdx ) {
    this->activeContourSeedIdx->DeepCopy( initVertIdx );
   }

  vtkPolyData *GetSource();

  /** return list of vertex indices of the 'dense curve'
    */
  vtkIntArray* GetActiveContour( ) {
    return this->activeContourVertIdx;
  }

protected:
  vtkInitClosedPath();
  ~vtkInitClosedPath() {}

  virtual int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *);
  virtual int FillInputPortInformation(int port, vtkInformation *info);

private:
  /** vertex indices of resulting initial active contour
   */
  vtkSmartPointer<vtkIntArray> activeContourVertIdx;

  /** vertex indices of resulting initial active contour
   */
  vtkSmartPointer<vtkFloatArray> distanceMapsFromSeeds;

  /** initial seed vertices
   */
  vtkSmartPointer<vtkIntArray> activeContourSeedIdx;

public:
  vtkInitClosedPath(const vtkInitClosedPath&);  // Not implemented.
  void operator=(const vtkInitClosedPath&);  // Not implemented.
};

#endif
