// Material Replacer
// -------------------------------------------------------------------
// Copyright (C) 2011 OpenEngine.dk (See AUTHORS) 
// 
// This program is free software; It is covered by the GNU General 
// Public License version 2 or any later version. 
// See the GNU General Public License for more details (see LICENSE). 
//--------------------------------------------------------------------

#include "MaterialReplacer.h"

#include <Geometry/Mesh.h>
#include <Logging/Logger.h>
#include <Scene/ISceneNode.h>
#include <Scene/MeshNode.h>

using namespace OpenEngine::Scene;
using namespace std;

namespace OpenEngine {
namespace Geometry {

    MaterialReplacer::Replacer::Replacer(ISceneNode* scene, const string oldMat, const MaterialPtr newMat)
        : oldMat(oldMat), newMat(newMat) {
        scene->Accept(*this);
    }

    void MaterialReplacer::Replacer::VisitMeshNode(MeshNode* node){
        MaterialPtr mat = node->GetMesh()->GetMaterial();
        logger.info << "node->GetMesh()->GetMaterial()->GetName(): " << mat->GetName() << ", addr: " << mat.get() << logger.end;
        if (mat->GetName().compare(oldMat) == 0) {
            logger.info << "Mat should be replaced" << logger.end;
            MeshPtr om = node->GetMesh();
            Mesh* newMesh = new Mesh(om->GetIndices(), om->GetType(), om->GetGeometrySet(), newMat, om->GetIndexOffset(), om->GetDrawingRange());
            node->SetMesh(MeshPtr(newMesh));
        }
        node->VisitSubNodes(*this);
    }

    void MaterialReplacer::InScene(ISceneNode* scene, const string oldMat, const MaterialPtr newMat){
        Replacer(scene, oldMat, newMat);
    }

}
}
