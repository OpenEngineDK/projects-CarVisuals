// Material Replacer
// -------------------------------------------------------------------
// Copyright (C) 2011 OpenEngine.dk (See AUTHORS) 
// 
// This program is free software; It is covered by the GNU General 
// Public License version 2 or any later version. 
// See the GNU General Public License for more details (see LICENSE). 
//--------------------------------------------------------------------

#ifndef _MATERIAL_REPLACER_H_
#define _MATERIAL_REPLACER_H_

#include <Geometry/Material.h>
#include <Scene/ISceneNodeVisitor.h>

namespace OpenEngine {
    namespace Scene {
        class ISceneNode;
        class MeshNode;
    }
namespace Geometry {

    class MaterialReplacer {
    private:      
        class Replacer : public virtual Scene::ISceneNodeVisitor {
            const std::string oldMat;
            const MaterialPtr newMat;
        public:
            Replacer(Scene::ISceneNode* scene, const std::string oldMat, const MaterialPtr newMat);
            void VisitMeshNode(Scene::MeshNode* node);
        };

    public:
        static void InScene(Scene::ISceneNode* scene, const std::string oldMat, const MaterialPtr newMat);
        
    };

}
}

#endif // _MATERIAL_REPLACER_H_
