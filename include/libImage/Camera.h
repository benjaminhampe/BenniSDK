// Copyright (C) 2002-2013 Benjamin Hampe
// This file is part of the "irrlicht-engine"
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __IRR_EXTENSION_CAMERA_H__
#define __IRR_EXTENSION_CAMERA_H__

#include <irrlicht.h>
#include <IGUIElement.h>

namespace irr
{

class Camera
{
public:
    typedef scene::ISceneNode TObject;
    typedef scene::ICameraSceneNode TCamera;
    typedef scene::ISceneManager TSceneManager;

    static TCamera* createFPS( TSceneManager* smgr, f32 move_speed, f32 turn_speed);

    static bool setActiveCamera( TSceneManager* smgr, TCamera* newActive );

    /// others
    static f32 getObjectDistance( TCamera* camera, TObject* node );

    static bool setToFollow( TCamera* camera, TObject* node, f32 dXZ, f32 dY );

    /// position
    static core::vector3df getPosition( TCamera* camera );

    static bool setPosition( TCamera* camera, const core::vector3df& pos );

    static bool setPosition( TCamera* camera, f32 x, f32 y, f32 z );

    static bool targetObject( TCamera* camera, TObject* node );


    /// rotation degrees
    static core::vector3df getRotation( TCamera* camera );

    static bool setRotation( TCamera* camera, const core::vector3df& pos );

    static bool setRotation( TCamera* camera, f32 x, f32 y, f32 z );

    static bool rotateX( TCamera* camera, f32 x );

    static bool rotateY( TCamera* camera, f32 y );

    static bool rotateZ( TCamera* camera, f32 z );

    /// target / eye
    static core::vector3df getTarget( TCamera* camera );

    static bool setTarget( TCamera* camera, const core::vector3df& pos );

    static bool setTarget( TCamera* camera, f32 x, f32 y, f32 z );

    /// move
    static bool moveCamera( TCamera* camera, f32 value );

    static bool moveCameraXZ( TCamera* camera, f32 value );

    static bool moveCameraY( TCamera* camera, f32 value );

    /// fly stuff
    static bool strafeCamera( TCamera* camera, f32 value );

    static bool turnCamera( TCamera* camera, f32 value );

    static bool tiltCamera( TCamera* camera, f32 value );

    static bool pitchCamera( TCamera* camera, f32 value );

    static bool rollCamera( TCamera* camera, f32 value );

    static bool yawCamera( TCamera* camera, f32 value );


    static bool viewOrtho( TCamera* camera );

    static bool viewIsometric( TCamera* camera );

    static bool viewDimetric( TCamera* camera );

    static bool viewPosX( TCamera* camera );

    static bool viewPosY( TCamera* camera );

    static bool viewPosZ( TCamera* camera );

    static bool viewNegX( TCamera* camera );

    static bool viewNegY( TCamera* camera );

    static bool viewNegZ( TCamera* camera );


    static core::vector3df getPickVector( TCamera* camera, s32 mouse_x, s32 mouse_y );

    static TObject* pickObject( TCamera* camera, s32 mouse_x, s32 mouse_y );

    // core::position2di getScreenPos( TSceneManager* smgr );
};

//
//class CameraManager : public gui::IGUIElement
//{
//public:
//    typedef scene::ISceneNode TObject;
//    typedef scene::ICameraSceneNode TCamera;
//    typedef scene::ISceneManager TSceneManager;
//
//    //! ctr
//    CameraManager( TSceneManager* smgr, gui::IGUIEnvironment* env, gui::IGUIElement* parent, s32 id, const core::recti& rectangle );
//
//    //! virtual dtr
//    virtual ~CameraManager();
//
//    //! virtual event handler
//    virtual bool OnEvent(const SEvent& event);
//
//    //! virtual render call ( element and its children )
//    virtual void draw();
//
//    bool clear();
//
//    u32 getCameraCount() const;
//
//    u32 addCamera();
//
//    TCamera* getCurrentCamera( );
//
//    bool setCurrentCamera( u32 index );
//
//    TCamera* getCamera( u32 index );
//
//    TCamera* getCameraByName( const core::stringw& _compare_name );
//
//    bool removeCamera( u32 index );
//
//    bool setCameraName( u32 index, const core::stringw& name );
//
//    core::stringw getCameraName( u32 index ) const;
//
//private:
//    TSceneManager* SceneManager;
//    core::array<TCamera*> Cameras;
//    TCamera* CurrentCamera;
//    core::array<core::stringw> CameraNames;
//};

}

////namespace irr{
////namespace scene{

//} // END NAMESPACE SCENE
//} // END NAMESPACE IRR

#endif // __IRR_C_DARKGL_CAMERA_SCENE_NODE_H__
