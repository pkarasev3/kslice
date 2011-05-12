/*
 * MATLAB Compiler: 4.13 (R2010a)
 * Date: Thu Sep 23 12:03:19 2010
 * Arguments: "-B" "macro_default" "-m" "-W" "main" "-T" "link:exe"
 * "defineCylinder.m" 
 */

#include "mclmcrrt.h"

#ifdef __cplusplus
extern "C" {
#endif
const unsigned char __MCC_defineCylinder_session_key[] = {
    '5', '6', '9', '6', 'A', 'D', 'B', '7', '0', 'E', '1', '8', 'C', 'C', '2',
    '3', 'D', '2', '3', '4', 'F', 'D', 'C', 'D', '3', '1', '3', 'D', '2', '7',
    'B', '4', '2', '2', '0', 'F', '0', 'B', '2', 'B', 'E', 'E', '2', 'B', 'E',
    'D', 'F', '5', '1', 'A', '0', 'C', '0', 'B', '8', '1', 'D', '5', '9', '7',
    'E', 'F', '8', '5', '5', '4', '7', 'C', '2', '1', '5', '2', '3', '5', '9',
    '9', 'C', '7', 'A', '2', '4', 'E', '6', 'F', 'B', 'A', 'B', '7', '8', '1',
    'F', '3', '8', 'F', '7', '1', '1', '1', 'D', '7', 'A', 'F', '2', 'A', '8',
    '9', '2', 'F', 'F', '5', '8', '3', '4', 'D', '2', 'C', '4', '6', '3', 'D',
    'D', '3', '9', 'D', '9', 'B', '7', 'A', '9', '8', '4', '1', 'A', 'A', '4',
    '7', 'B', 'D', '8', '0', '9', 'F', 'B', 'F', '4', '9', '8', '5', '9', 'E',
    '5', 'C', '7', '3', 'B', 'C', '5', 'C', '5', 'D', '1', 'B', 'B', '3', '2',
    'E', 'A', '6', '0', 'F', '5', '1', '7', '6', '8', 'E', '2', '2', 'D', 'A',
    '1', '2', 'B', 'A', 'F', 'D', '5', '3', '9', '3', '8', '3', '8', 'F', 'E',
    '7', '4', '3', '1', 'A', '1', 'F', '8', 'E', '4', '4', '2', '7', '9', '2',
    'C', 'B', '9', 'A', '5', '4', '9', '9', '3', '2', '7', '1', 'A', 'A', '7',
    '0', 'F', '6', 'E', '7', '7', 'E', '7', '4', '6', 'C', '3', '7', 'A', '7',
    '6', '9', '2', 'E', 'F', '3', 'A', '7', 'B', '8', '5', 'E', '6', 'A', 'B',
    'E', '\0'};

const unsigned char __MCC_defineCylinder_public_key[] = {
    '3', '0', '8', '1', '9', 'D', '3', '0', '0', 'D', '0', '6', '0', '9', '2',
    'A', '8', '6', '4', '8', '8', '6', 'F', '7', '0', 'D', '0', '1', '0', '1',
    '0', '1', '0', '5', '0', '0', '0', '3', '8', '1', '8', 'B', '0', '0', '3',
    '0', '8', '1', '8', '7', '0', '2', '8', '1', '8', '1', '0', '0', 'C', '4',
    '9', 'C', 'A', 'C', '3', '4', 'E', 'D', '1', '3', 'A', '5', '2', '0', '6',
    '5', '8', 'F', '6', 'F', '8', 'E', '0', '1', '3', '8', 'C', '4', '3', '1',
    '5', 'B', '4', '3', '1', '5', '2', '7', '7', 'E', 'D', '3', 'F', '7', 'D',
    'A', 'E', '5', '3', '0', '9', '9', 'D', 'B', '0', '8', 'E', 'E', '5', '8',
    '9', 'F', '8', '0', '4', 'D', '4', 'B', '9', '8', '1', '3', '2', '6', 'A',
    '5', '2', 'C', 'C', 'E', '4', '3', '8', '2', 'E', '9', 'F', '2', 'B', '4',
    'D', '0', '8', '5', 'E', 'B', '9', '5', '0', 'C', '7', 'A', 'B', '1', '2',
    'E', 'D', 'E', '2', 'D', '4', '1', '2', '9', '7', '8', '2', '0', 'E', '6',
    '3', '7', '7', 'A', '5', 'F', 'E', 'B', '5', '6', '8', '9', 'D', '4', 'E',
    '6', '0', '3', '2', 'F', '6', '0', 'C', '4', '3', '0', '7', '4', 'A', '0',
    '4', 'C', '2', '6', 'A', 'B', '7', '2', 'F', '5', '4', 'B', '5', '1', 'B',
    'B', '4', '6', '0', '5', '7', '8', '7', '8', '5', 'B', '1', '9', '9', '0',
    '1', '4', '3', '1', '4', 'A', '6', '5', 'F', '0', '9', '0', 'B', '6', '1',
    'F', 'C', '2', '0', '1', '6', '9', '4', '5', '3', 'B', '5', '8', 'F', 'C',
    '8', 'B', 'A', '4', '3', 'E', '6', '7', '7', '6', 'E', 'B', '7', 'E', 'C',
    'D', '3', '1', '7', '8', 'B', '5', '6', 'A', 'B', '0', 'F', 'A', '0', '6',
    'D', 'D', '6', '4', '9', '6', '7', 'C', 'B', '1', '4', '9', 'E', '5', '0',
    '2', '0', '1', '1', '1', '\0'};

static const char * MCC_defineCylinder_matlabpath_data[] = 
  { "defineCylind/", "$TOOLBOXDEPLOYDIR/", "$TOOLBOXMATLABDIR/general/",
    "$TOOLBOXMATLABDIR/ops/", "$TOOLBOXMATLABDIR/lang/",
    "$TOOLBOXMATLABDIR/elmat/", "$TOOLBOXMATLABDIR/randfun/",
    "$TOOLBOXMATLABDIR/elfun/", "$TOOLBOXMATLABDIR/specfun/",
    "$TOOLBOXMATLABDIR/matfun/", "$TOOLBOXMATLABDIR/datafun/",
    "$TOOLBOXMATLABDIR/polyfun/", "$TOOLBOXMATLABDIR/funfun/",
    "$TOOLBOXMATLABDIR/sparfun/", "$TOOLBOXMATLABDIR/scribe/",
    "$TOOLBOXMATLABDIR/graph2d/", "$TOOLBOXMATLABDIR/graph3d/",
    "$TOOLBOXMATLABDIR/specgraph/", "$TOOLBOXMATLABDIR/graphics/",
    "$TOOLBOXMATLABDIR/uitools/", "$TOOLBOXMATLABDIR/strfun/",
    "$TOOLBOXMATLABDIR/imagesci/", "$TOOLBOXMATLABDIR/iofun/",
    "$TOOLBOXMATLABDIR/audiovideo/", "$TOOLBOXMATLABDIR/timefun/",
    "$TOOLBOXMATLABDIR/datatypes/", "$TOOLBOXMATLABDIR/verctrl/",
    "$TOOLBOXMATLABDIR/codetools/", "$TOOLBOXMATLABDIR/helptools/",
    "$TOOLBOXMATLABDIR/demos/", "$TOOLBOXMATLABDIR/timeseries/",
    "$TOOLBOXMATLABDIR/hds/", "$TOOLBOXMATLABDIR/guide/",
    "$TOOLBOXMATLABDIR/plottools/", "toolbox/local/",
    "$TOOLBOXMATLABDIR/datamanager/", "toolbox/compiler/" };

static const char * MCC_defineCylinder_classpath_data[] = 
  { "" };

static const char * MCC_defineCylinder_libpath_data[] = 
  { "" };

static const char * MCC_defineCylinder_app_opts_data[] = 
  { "" };

static const char * MCC_defineCylinder_run_opts_data[] = 
  { "" };

static const char * MCC_defineCylinder_warning_state_data[] = 
  { "off:MATLAB:dispatcher:nameConflict" };


mclComponentData __MCC_defineCylinder_component_data = { 

  /* Public key data */
  __MCC_defineCylinder_public_key,

  /* Component name */
  "defineCylinder",

  /* Component Root */
  "",

  /* Application key data */
  __MCC_defineCylinder_session_key,

  /* Component's MATLAB Path */
  MCC_defineCylinder_matlabpath_data,

  /* Number of directories in the MATLAB Path */
  37,

  /* Component's Java class path */
  MCC_defineCylinder_classpath_data,
  /* Number of directories in the Java class path */
  0,

  /* Component's load library path (for extra shared libraries) */
  MCC_defineCylinder_libpath_data,
  /* Number of directories in the load library path */
  0,

  /* MCR instance-specific runtime options */
  MCC_defineCylinder_app_opts_data,
  /* Number of MCR instance-specific runtime options */
  0,

  /* MCR global runtime options */
  MCC_defineCylinder_run_opts_data,
  /* Number of MCR global runtime options */
  0,
  
  /* Component preferences directory */
  "defineCylind_289F345383A38D09C92F7CE259E9FB75",

  /* MCR warning status data */
  MCC_defineCylinder_warning_state_data,
  /* Number of MCR warning status modifiers */
  1,

  /* Path to component - evaluated at runtime */
  NULL

};

#ifdef __cplusplus
}
#endif


