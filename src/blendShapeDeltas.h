#ifndef BLENDSHAPEDELTAS_H
#define BLENDSHAPEDELTAS_H



#include <maya/MFnPlugin.h>
#include <maya/MPxCommand.h>
#include <maya/MArgList.h>
#include <maya/MGlobal.h>
#include <maya/MSelectionList.h>
#include <maya/MObject.h>
#include <maya/MFnMesh.h>
#include <maya/MPointArray.h>
#include <maya/MFnBlendShapeDeformer.h>
#include <maya/MFnDependencyNode.h>
#include <maya/MPlug.h>
#include <maya/MArrayDataHandle.h>
#include <maya/MFnPointArrayData.h>
#include <maya/MFnComponentListData.h>
#include <maya/MFnSingleIndexedComponent.h>


#include <string>
using namespace std;


//
// Class Declarations

class SetBlendShapeDeltasCmd : public MPxCommand
{
public:
    SetBlendShapeDeltasCmd() : targetIndex(-1) {}
    virtual MStatus doIt(const MArgList& args);
    virtual MStatus redoIt();
    virtual MStatus undoIt();
    virtual bool isUndoable() const { return true; }
    static void* creator() { return new SetBlendShapeDeltasCmd(); }

private:
    MString sourceMeshName;
    MString targetMeshName;
    MString blendShapeNodeName;
    int targetIndex;
    MPointArray originalPoints;
    MPointArray deltaPoints;

    MObject sourceMeshObj;
    MObject targetMeshObj;
    MObject blendShapeNodeObj;
    MObject originalComponentsDataObj;
};

#endif


