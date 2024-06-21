//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Include
#include "blendShapeDeltas.h" 


MStatus SetBlendShapeDeltasCmd::doIt(const MArgList& args)
{
    if (args.length() < 4) {
        MGlobal::displayError("Usage: setBlendShapeDeltas <sourceMesh> <targetMesh> <blendShapeNode> <targetIndex>");
        return MS::kFailure;
    }

    sourceMeshName = args.asString(0);
    targetMeshName = args.asString(1);
    blendShapeNodeName = args.asString(2);
    targetIndex = args.asInt(3);

    MSelectionList selection;

    selection.add(sourceMeshName);
    selection.getDependNode(0, sourceMeshObj);

    selection.add(targetMeshName);
    selection.getDependNode(1, targetMeshObj);

    selection.add(blendShapeNodeName);
    selection.getDependNode(2, blendShapeNodeObj);

    MStatus status;
    MFnMesh sourceMeshFn(sourceMeshObj, &status);
    if (!status) {
        MGlobal::displayError("Failed to get source mesh.");
        return status;
    }

    MFnMesh targetMeshFn(targetMeshObj, &status);
    if (!status) {
        MGlobal::displayError("Failed to get target mesh.");
        return status;
    }

    MFnBlendShapeDeformer blendShapeFn(blendShapeNodeObj, &status);
    if (!status) {
        MGlobal::displayError("Failed to get blendShape node.");
        return status;
    }

    MPointArray sourcePoints, targetPoints;
    sourceMeshFn.getPoints(sourcePoints);
    targetMeshFn.getPoints(targetPoints);

    if (sourcePoints.length() != targetPoints.length()) {
        MGlobal::displayError("Source and target meshes must have the same number of vertices.");
        return MS::kFailure;
    }

    deltaPoints.setLength(sourcePoints.length());
    //MGlobal::displayInfo(MString("source_points length: ") + sourcePoints.length());
    for (unsigned int i = 0; i < sourcePoints.length(); ++i) {
        deltaPoints[i] = targetPoints[i] - sourcePoints[i];
        //MGlobal::displayInfo(MString("delta index ") + i + " = " + deltaPoints[i].x + "," + deltaPoints[i].y + "," + deltaPoints[i].z);
    }

    // Retrieve the target plug for the blendShape node
    MFnDependencyNode depNodeFn(blendShapeNodeObj);
    MPlug inputTargetPlug = depNodeFn.findPlug("inputTarget", true, &status);
    if (!status) {
        MGlobal::displayError("Failed to find inputTarget plug.");
        return status;
    }

    // Navigate to the specific target index
    MPlug targetIndexPlug = inputTargetPlug.elementByLogicalIndex(0).child(0).elementByLogicalIndex(targetIndex).child(0).elementByPhysicalIndex(0);
    MPlug inputPointsPlug = targetIndexPlug.child(3); 
    MPlug inputComponentsPlug = targetIndexPlug.child(4);// Assuming child 3 is the inputPoints
    //MGlobal::displayInfo(targetIndexPlug.partialName(true));
    //MGlobal::displayInfo(inputPointsPlug.partialName(true));
    //MGlobal::displayInfo(MString("Plug Elements: ") + inputPointsPlug.numElements());
    
    

    // Store the original points for undo
    MObject pointArrayObj;
    MObject originalComponentsDataObj;
    inputPointsPlug.getValue(pointArrayObj);
    inputComponentsPlug.getValue(originalComponentsDataObj);
    MFnPointArrayData pointArrayData(pointArrayObj);
    originalPoints = pointArrayData.array();

    return redoIt();
}


MStatus SetBlendShapeDeltasCmd::redoIt()
{
    MFnDependencyNode depNodeFn(blendShapeNodeObj);
    MStatus status;
    MPlug inputTargetPlug = depNodeFn.findPlug("inputTarget", true, &status);
    if (!status) {
        MGlobal::displayError("Failed to find inputTarget plug.");
        return status;
    }

    MPlug targetIndexPlug = inputTargetPlug.elementByLogicalIndex(0).child(0).elementByLogicalIndex(targetIndex).child(0).elementByPhysicalIndex(0);
    MPlug inputPointsPlug = targetIndexPlug.child(3);
    MPlug inputComponentsPlug = targetIndexPlug.child(4);

    // Create an MFnComponentListData object for the components
    MFnComponentListData componentListDataFn;
    MObject componentsDataObj = componentListDataFn.create();
    MFnSingleIndexedComponent singleIndexedComponentFn;
    MObject vertexComponent = singleIndexedComponentFn.create(MFn::kMeshVertComponent);

    MIntArray vertexIndices(deltaPoints.length());
    for (unsigned int i = 0; i < deltaPoints.length(); ++i) {
        vertexIndices[i] = i;
    }
    singleIndexedComponentFn.addElements(vertexIndices);
    componentListDataFn.add(vertexComponent);

    // Set the delta points on the inputPoints plug using MFnPointArrayData
    MFnPointArrayData pointArrayDataFn;
    //MGlobal::displayInfo(MString("delta points length: ") + deltaPoints.length());
    MGlobal::displayInfo(MString(inputComponentsPlug.name()));
    MObject deltaPointsDataObj = pointArrayDataFn.create(deltaPoints);
    inputPointsPlug.setValue(deltaPointsDataObj);

    // Set the components using MDataHandle
    MDataHandle dataHandle = inputComponentsPlug.asMDataHandle();
    dataHandle.set(componentsDataObj);
    inputComponentsPlug.setMObject(componentsDataObj);

    return MS::kSuccess;
}

MStatus SetBlendShapeDeltasCmd::undoIt()
{
    MFnDependencyNode depNodeFn(blendShapeNodeObj);
    MStatus status;
    MPlug inputTargetPlug = depNodeFn.findPlug("inputTarget", true, &status);
    if (!status) {
        MGlobal::displayError("Failed to find inputTarget plug.");
        return status;
    }

    MPlug targetIndexPlug = inputTargetPlug.elementByLogicalIndex(0).child(0).elementByLogicalIndex(targetIndex).child(0).elementByPhysicalIndex(0);
    MPlug inputPointsPlug = targetIndexPlug.child(3);
    MPlug inputComponentsPlug = targetIndexPlug.child(4);

    // Restore the original points on the inputPoints plug using MFnPointArrayData
    MFnPointArrayData pointArrayDataFn;
    MObject originalPointsDataObj = pointArrayDataFn.create(originalPoints);
    inputPointsPlug.setValue(originalPointsDataObj);
    // Set the components using MDataHandle
    MDataHandle dataHandle = inputComponentsPlug.asMDataHandle();
    dataHandle.set(originalComponentsDataObj);
    inputComponentsPlug.setMObject(originalComponentsDataObj);

    return MS::kSuccess;
}

MStatus initializePlugin(MObject obj)
{
    MFnPlugin plugin(obj, "Squarebit", "1.0", "Any");
    plugin.registerCommand("setBlendShapeDeltas", SetBlendShapeDeltasCmd::creator);
    return MS::kSuccess;
}

MStatus uninitializePlugin(MObject obj)
{
    MFnPlugin plugin(obj);
    plugin.deregisterCommand("setBlendShapeDeltas");
    return MS::kSuccess;
}