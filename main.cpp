// main
// -------------------------------------------------------------------
// Copyright (C) 2011 OpenEngine.dk (See AUTHORS) 
// 
// This program is free software; It is covered by the GNU General 
// Public License version 2 or any later version. 
// See the GNU General Public License for more details (see LICENSE). 
//--------------------------------------------------------------------

// OpenEngine stuff
#include <Core/Engine.h>
#include <Display/Camera.h>
#include <Display/Frustum.h>
#include <Display/PerspectiveViewingVolume.h>
#include <Display/RenderCanvas.h>
#include <Logging/Logger.h>
#include <Logging/ColorStreamLogger.h>
#include <Geometry/Material.h>
#include <Geometry/Mesh.h>
#include <Meta/Config.h>
#include <Renderers/TextureLoader.h>
#include <Resources/Cubemap.h>
#include <Resources/Directory.h>
#include <Resources/ITexture.h>
#include <Resources/IModelResource.h>
#include <Resources/ResourceManager.h>
#include <Scene/DotVisitor.h>
#include <Scene/MeshNode.h>
#include <Scene/SceneNode.h>

#include <Utils/MeshCreator.h>

#include <Utils/BetterMoveHandler.h>

// SDL
#include <Display/SDLEnvironment.h>
#include <Resources/SDLImage.h>

#include <Resources/FreeImage.h>
#include <Resources/AssimpResource.h>


// OpenGL stuff
#include <Display/OpenGL/TextureCopy.h>
#include <Renderers/OpenGL/Renderer.h>
#include <Renderers/OpenGL/RenderingView.h>
#include <Renderers/OpenGL/ShaderLoader.h>
#include <Resources/OpenGLShader.h>

// Project
#include "Geometry/MaterialReplacer.h"

// name spaces that we will be using.
// this combined with the above imports is almost the same as
// fx. import OpenEngine.Logging.*; in Java.
using namespace OpenEngine;
using namespace OpenEngine::Core;
using namespace OpenEngine::Display;
using namespace OpenEngine::Display::OpenGL;
using namespace OpenEngine::Logging;
using namespace OpenEngine::Geometry;
using namespace OpenEngine::Math;
using namespace OpenEngine::Resources;
using namespace OpenEngine::Renderers;
using namespace OpenEngine::Renderers::OpenGL;
using namespace OpenEngine::Utils;
using namespace OpenEngine::Utils::MeshCreator;

class TextureLoadOnInit
    : public IListener<RenderingEventArg> {
    TextureLoader& tl;
public:
    TextureLoadOnInit(TextureLoader& tl) : tl(tl) { }
    void Handle(RenderingEventArg arg) {
        if (arg.canvas.GetScene() != NULL)
            tl.Load(*arg.canvas.GetScene());
    }
};

class QuitHandler : public IListener<KeyboardEventArg> {
    IEngine& engine;
public:
    QuitHandler(IEngine& engine) : engine(engine) {}
    void Handle(KeyboardEventArg arg) {
        if (arg.sym == KEY_ESCAPE) engine.Stop();
    }
};

class SkyboxModule : public IListener<Core::ProcessEventArg> {
private:
    ICubemapPtr skybox;
    IViewingVolume& view;
    TransformationNode* tNode;
    
public:
    SkyboxModule(ICubemapPtr skybox, IRenderCanvas* canvas)
        : skybox(skybox), view((*canvas->GetViewingVolume())) {
        tNode = new TransformationNode();
        
        MeshPtr cube = CreateCube(2000, 1, Vector<3,float>(1,1,1), true);
        cube->GetMaterial()->shad = ResourceManager<IShaderResource>::Create("shaders/skybox.glsl");
        cube->GetMaterial()->shad->SetTexture("skybox", skybox);
        
        MeshNode* mNode = new MeshNode(cube);
        tNode->AddNode(mNode);
        canvas->GetScene()->AddNode(tNode);
    }

    void Handle(Core::ProcessEventArg arg) {
        // Move skybox to camera
        Vector<3,float> pos = view.GetPosition();
        // pos[1] = 1000.0f;
        tNode->SetPosition(pos);
    }

};

/**
 * Main method for the first quarter project of CGD.
 * Corresponds to the
 *   public static void main(String args[])
 * method in Java.
 */
int main(int argc, char** argv) {
    // Create simple setup
    //SimpleSetup* setup = new SimpleSetup("Car Visuals");
    Logger::AddLogger(new ColorStreamLogger(&std::cout));

    logger.info << "========= Running OpenEngine Test Project =========" << logger.end;

    IEngine* engine = new Engine();

    IEnvironment* env = new SDLEnvironment(1024,768, 32);
    engine->InitializeEvent().Attach(*env);
    engine->ProcessEvent().Attach(*env);
    engine->DeinitializeEvent().Attach(*env);

    Camera* camera  = new Camera(*(new PerspectiveViewingVolume(1,4000)));
    Frustum* frustum = new Frustum(*camera);
    IRenderCanvas* canvas = new RenderCanvas(new TextureCopy());
    canvas->SetViewingVolume(frustum);
    canvas->SetScene(new SceneNode());
    IFrame* frame = &env->CreateFrame();
    frame->SetCanvas(canvas);
 
    IRenderer* renderer = new Renderer();
    canvas->SetRenderer(renderer);
    RenderingView* rv = new RenderingView();
    renderer->ProcessEvent().Attach(*rv);
    renderer->InitializeEvent().Attach(*rv);
    TextureLoader* textureloader = new TextureLoader(*renderer);
    renderer->InitializeEvent().Attach(*(new TextureLoadOnInit(*textureloader)));
    renderer->PreProcessEvent().Attach(*textureloader);

    ResourceManager<ITexture2D>::AddPlugin(new FreeImagePlugin());
    ResourceManager<IShaderResource>::AddPlugin(new GLShaderPlugin());
    ResourceManager<IModelResource>::AddPlugin(new AssimpPlugin());
    DirectoryManager::AppendPath("projects/CarVisuals/");
    DirectoryManager::AppendPath("resources/");

    ICubemapPtr map = Cubemap::Create(2048, RGBA, true);

    ITexture2DPtr negx = ResourceManager<ITexture2D>::Create("SaintLazarusChurch/negx.jpg");
    negx->Load();
    map->SetPixels(negx, ICubemap::NEGATIVE_X);
    ITexture2DPtr posx = ResourceManager<ITexture2D>::Create("SaintLazarusChurch/posx.jpg");
    posx->Load();
    map->SetPixels(posx, ICubemap::POSITIVE_X);

    ITexture2DPtr negy = ResourceManager<ITexture2D>::Create("SaintLazarusChurch/posy.jpg");
    negy->Load();
    map->SetPixels(negy, ICubemap::NEGATIVE_Y);
    ITexture2DPtr posy = ResourceManager<ITexture2D>::Create("SaintLazarusChurch/negy.jpg");
    posy->Load();
    map->SetPixels(posy, ICubemap::POSITIVE_Y);

    ITexture2DPtr negz = ResourceManager<ITexture2D>::Create("SaintLazarusChurch/negz.jpg");
    negz->Load();
    map->SetPixels(negz, ICubemap::NEGATIVE_Z);
    ITexture2DPtr posz = ResourceManager<ITexture2D>::Create("SaintLazarusChurch/posz.jpg");
    posz->Load();
    map->SetPixels(posz, ICubemap::POSITIVE_Z);

    ICubemap::GenerateMipmaps(map);


    SkyboxModule* skyMod = new SkyboxModule(map, canvas);
    engine->ProcessEvent().Attach(*skyMod);

    MeshPtr sphere = CreateSphere(2, 25, Vector<3,float>(0,0,1));
    sphere->GetMaterial()->shad = ResourceManager<IShaderResource>::Create("shaders/cubemap.glsl");
    sphere->GetMaterial()->shad->SetTexture("environment", map);

    // Car model can be found on OE dropbox
    IModelResourcePtr car = ResourceManager<IModelResource>::Create("resources/AudiR8/AudiR8.dae");
    car->Load();

    TransformationNode* trans = new TransformationNode();
    trans->Move(0,0,-10);
    //ISceneNode* sphereNode = new MeshNode(sphere);
    ISceneNode* sphereNode = car->GetSceneNode();
    
    
    

    canvas->GetScene()->AddNode(trans);
    trans->AddNode(sphereNode);

    MaterialReplacer::InScene(canvas->GetScene(), "CarPaint", sphere->GetMaterial());

    
    textureloader->Load(map);
    
    BetterMoveHandler *move = new BetterMoveHandler(*camera,
                                                    *(env->GetMouse()),
                                                    true);
    engine->InitializeEvent().Attach(*move);
    engine->ProcessEvent().Attach(*move);
	
	env->GetKeyboard()->KeyEvent().Attach(*move);   
    env->GetMouse()->MouseButtonEvent().Attach(*move);
    env->GetMouse()->MouseMovedEvent().Attach(*move);
    
    env->GetKeyboard()->KeyEvent().Attach(*(new QuitHandler(*engine)));

    // DotVisitor* dot = new DotVisitor("Scene");
    // logger.info << dot->String(*(canvas->GetScene())) << logger.end;

    // Start the engine.
    engine->Start();

    // Return when the engine stops.
    return EXIT_SUCCESS;
}


