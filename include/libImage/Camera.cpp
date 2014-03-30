#include "Camera.h"

#include <irrlicht.h>
#include <IGUIElement.h>
#include <../source/Irrlicht/os.h>

//#include "gui/CGUIVector3d.h"

namespace irr
{

// SKeyMap Camera::FPSKeyMap[6];
//Camera::FPSKeyMap[0].Action = EKA_MOVE_FORWARD;
//Camera::FPSKeyMap[1].Action = EKA_MOVE_BACKWARD;
//Camera::FPSKeyMap[2].Action = EKA_STRAFE_LEFT;
//Camera::FPSKeyMap[3].Action = EKA_STRAFE_RIGHT;
//Camera::FPSKeyMap[4].Action = EKA_CROUCH;
//Camera::FPSKeyMap[5].Action = EKA_JUMP_UP;
//Camera::FPSKeyMap[0].KeyCode = KEY_KEY_W;
//Camera::FPSKeyMap[1].KeyCode = KEY_KEY_S;
//Camera::FPSKeyMap[2].KeyCode = KEY_KEY_A;
//Camera::FPSKeyMap[3].KeyCode = KEY_KEY_D;
//Camera::FPSKeyMap[4].KeyCode = KEY_KEY_C;
//Camera::FPSKeyMap[5].KeyCode = KEY_SPACE;

Camera::TCamera* Camera::createFPS( TSceneManager* smgr, f32 move_speed, f32 turn_speed)
{
	if (!smgr)
		return 0;

	SKeyMap KeyMapArray[6];
	KeyMapArray[0].Action = EKA_MOVE_FORWARD;
	KeyMapArray[1].Action = EKA_MOVE_BACKWARD;
	KeyMapArray[2].Action = EKA_STRAFE_LEFT;
	KeyMapArray[3].Action = EKA_STRAFE_RIGHT;
	KeyMapArray[4].Action = EKA_CROUCH;
	KeyMapArray[5].Action = EKA_JUMP_UP;
	KeyMapArray[0].KeyCode = KEY_KEY_W;
	KeyMapArray[1].KeyCode = KEY_KEY_S;
	KeyMapArray[2].KeyCode = KEY_KEY_A;
	KeyMapArray[3].KeyCode = KEY_KEY_D;
	KeyMapArray[4].KeyCode = KEY_KEY_C;
	KeyMapArray[5].KeyCode = KEY_SPACE;

	TCamera* camera = smgr->addCameraSceneNodeFPS( smgr->getRootSceneNode(), turn_speed, move_speed, -1, KeyMapArray, 6);
	if (camera)
	{
		camera->setNearValue( 0.01f );
		camera->setFarValue( 10000.0f );
		camera->setPosition( core::vector3df( 0,100,0) );
		camera->setTarget( core::vector3df(0,0,100) );
	}
	return camera;
}



//u32 Camera::addCameraFPS( f32 move_speed, f32 turn_speed )
//{
//    if (!SceneManager)
//        return 0;
//
//    u32 index = 0;
//
//	SKeyMap KeyMapArray[6];
//	KeyMapArray[0].Action = EKA_MOVE_FORWARD;
//	KeyMapArray[1].Action = EKA_MOVE_BACKWARD;
//	KeyMapArray[2].Action = EKA_STRAFE_LEFT;
//	KeyMapArray[3].Action = EKA_STRAFE_RIGHT;
//	KeyMapArray[4].Action = EKA_CROUCH;
//	KeyMapArray[5].Action = EKA_JUMP_UP;
//	KeyMapArray[0].KeyCode = KEY_KEY_W;
//	KeyMapArray[1].KeyCode = KEY_KEY_S;
//	KeyMapArray[2].KeyCode = KEY_KEY_A;
//	KeyMapArray[3].KeyCode = KEY_KEY_D;
//	KeyMapArray[4].KeyCode = KEY_KEY_C;
//	KeyMapArray[5].KeyCode = KEY_SPACE;
//
//	TCamera* camera = SceneManager->addCameraSceneNodeFPS( SceneManager->getRootSceneNode(), turn_speed, move_speed, -1, KeyMapArray, 6);
//	if (camera)
//	{
//		camera->setNearValue( 0.01f );
//		camera->setFarValue( 10000.0f );
//		camera->setPosition( core::vector3df( 0,100,0) );
//		camera->setTarget( core::vector3df(0,0,100) );
//
//		core::dimension2du screenSize = SceneManager->getVideoDriver()->getCurrentRenderTargetSize();
//		camera->setAspectRatio( (f32)screenSize.Width / (f32)screenSize.Height );
//        CurrentCamera = camera;
//
//        Cameras.push_back( camera );
//        index = Cameras.size();
//	}
//	return index;
//}


bool Camera::setActiveCamera( TSceneManager* smgr, TCamera* newActive )
{
	if (!smgr)
		return false;

	TCamera* active = smgr->getActiveCamera();
	if (active)
        active->setInputReceiverEnabled(false);

    if (newActive)
        newActive->setInputReceiverEnabled(true);

	smgr->setActiveCamera(newActive);
	return true;
}

/// others
f32 Camera::getObjectDistance( TCamera* camera, TObject* node )
{
    return 0.0f;
}

bool Camera::setToFollow( TCamera* camera, TObject* node, f32 dXZ, f32 dY )
{
    if (!camera)
        return false;

    /// do something ...

    return true;
}


///////////////////////////////////////////////////////////////////////////////
// set a perspective frustum with 6 params similar to glFrustum()
// (left, right, bottom, top, near, far)
// Note: this is for row-major notation. OpenGL needs transpose it
///////////////////////////////////////////////////////////////////////////////
//bool ModelGL::setFrustum(float l, float r, float b, float t, float n, float f)
//{
//    matrixProjection.identity();
//    matrixProjection[0]  = 2 * n / (r - l);
//    matrixProjection[2]  = (r + l) / (r - l);
//    matrixProjection[5]  = 2 * n / (t - b);
//    matrixProjection[6]  = (t + b) / (t - b);
//    matrixProjection[10] = -(f + n) / (f - n);
//    matrixProjection[11] = -(2 * f * n) / (f - n);
//    matrixProjection[14] = -1;
//    matrixProjection[15] = 0;
//}
//
///////////////////////////////////////////////////////////////////////////////
// set a orthographic frustum with 6 params similar to glOrtho()
// (left, right, bottom, top, near, far)
// Note: this is for row-major notation. OpenGL needs transpose it
///////////////////////////////////////////////////////////////////////////////
//bool ModelGL::setOrthoFrustum(float l, float r, float b, float t, float n,
//                              float f)
//{
//    matrixProjection.identity();
//    matrixProjection[0]  = 2 / (r - l);
//    matrixProjection[3]  = -(r + l) / (r - l);
//    matrixProjection[5]  = 2 / (t - b);
//    matrixProjection[7]  = -(t + b) / (t - b);
//    matrixProjection[10] = -2 / (f - n);
//    matrixProjection[11] = -(f + n) / (f - n);
//}
//...
//
//// pass projection matrx to OpenGL before draw
//glMatrixMode(GL_PROJECTION);
//glLoadMatrixf(matrixProjection.getTranspose());
//...

/// position
core::vector3df Camera::getPosition( TCamera* camera )
{
    return core::vector3df(0,0,0);
}

bool Camera::setPosition( TCamera* camera, const core::vector3df& pos )
{
    if (!camera)
        return false;

    /// do something ...

    return true;
}

bool Camera::setPosition( TCamera* camera, f32 x, f32 y, f32 z )
{
    if (!camera)
        return false;

    /// do something ...

    return true;
}

/// move
bool Camera::moveCamera( TCamera* camera, f32 value )
{
    if (!camera)
        return false;

    /// do something ...

    return true;
}

bool Camera::moveCameraXZ( TCamera* camera, f32 value )
{
    if (!camera)
        return false;

    /// do something ...

    return true;
}

bool Camera::moveCameraY( TCamera* camera, f32 value )
{
    if (!camera)
        return false;

    /// do something ...

    return true;
}

/// rotation degrees
core::vector3df Camera::getRotation( TCamera* camera )
{
	return core::vector3df(0,0,0);
}

bool Camera::setRotation( TCamera* camera, const core::vector3df& pos )
{
    if (!camera)
        return false;

    /// do something ...

    return true;
}

bool Camera::setRotation( TCamera* camera, f32 x, f32 y, f32 z )
{
    if (!camera)
        return false;

    /// do something ...

    return true;
}

//bool Camera::rotate( TCamera* camera, const core::vector3df& pos )
//{
//    if (!camera)
//        return false;
//
//    /// do something ...
//
//    return true;
//}
//
//bool Camera::rotate( TCamera* camera, f32 x, f32 y, f32 z )
//{
//    if (!camera)
//        return false;
//
//    /// do something ...
//
//    return true;
//}

bool Camera::rotateX( TCamera* camera, f32 x )
{
    if (!camera)
        return false;

    /// do something ...

    return true;
}

bool Camera::rotateY( TCamera* camera, f32 y )
{
    if (!camera)
        return false;

    /// do something ...

    return true;
}

bool Camera::rotateZ( TCamera* camera, f32 z )
{
    if (!camera)
        return false;

    /// do something ...

    return true;
}

/// target / eye
core::vector3df Camera::getTarget( TCamera* camera )
{
    return core::vector3df(0,0,0);
}

bool Camera::setTarget( TCamera* camera, const core::vector3df& pos )
{
    if (!camera)
        return false;

    /// do something ...

    return true;
}

bool Camera::setTarget( TCamera* camera, f32 x, f32 y, f32 z )
{
    if (!camera)
        return false;

    /// do something ...

    return true;
}


bool Camera::targetObject( TCamera* camera, TObject* node )
{
    if (!node)
        return false;

    if (!camera)
        return false;

    core::vector3df eye = node->getTransformedBoundingBox().getCenter();

    camera->setTarget( eye );
    return true;
}

/// others
bool Camera::strafeCamera( TCamera* camera, f32 value )
{
    if (!camera)
        return false;

    /// do something ...

    return true;
}

bool Camera::turnCamera( TCamera* camera, f32 value )
{
    if (!camera)
        return false;

    /// do something ...

    return true;
}

bool Camera::tiltCamera( TCamera* camera, f32 value )
{
    if (!camera)
        return false;

    /// do something ...

    return true;
}

bool Camera::pitchCamera( TCamera* camera, f32 value )
{
    if (!camera)
        return false;

    /// do something ...

    return true;
}

bool Camera::rollCamera( TCamera* camera, f32 value )
{
    if (!camera)
        return false;

    /// do something ...

    return true;
}

bool Camera::yawCamera( TCamera* camera, f32 value )
{
    if (!camera)
        return false;

    /// do something ...

    return true;
}

core::vector3df Camera::getPickVector( TCamera* camera, s32 mouse_x, s32 mouse_y )
{
    return core::vector3df(0,0,0);
}

Camera::TObject* Camera::pickObject( TCamera* camera, s32 mouse_x, s32 mouse_y )
{
    return 0;
}

bool Camera::viewOrtho( TCamera* camera )
{
	if (!camera)
		return false;

	camera->setPosition( core::vector3df( 0,100,0) );
	camera->setTarget( core::vector3df(0,0,100) );
	return true;
}

bool Camera::viewIsometric( TCamera* camera )
{
	if (!camera)
		return false;

	camera->setPosition( core::vector3df( 0,100,0) );
	camera->setTarget( core::vector3df(0,0,100) );
	return true;
}

bool Camera::viewDimetric( TCamera* camera )
{
	if (!camera)
		return false;

	camera->setPosition( core::vector3df( 0,100,0) );
	camera->setTarget( core::vector3df(0,0,100) );
	return true;
}

bool Camera::viewPosX( TCamera* camera )
{
	if (!camera)
		return false;

	camera->setPosition( core::vector3df( 0,100,0) );
	camera->setTarget( core::vector3df(0,0,100) );
	return true;
}

bool Camera::viewPosY( TCamera* camera )
{
	if (!camera)
		return false;

	camera->setPosition( core::vector3df( 0,100,0) );
	camera->setTarget( core::vector3df(0,0,100) );
	return true;
}

bool Camera::viewPosZ( TCamera* camera )
{
	if (!camera)
		return false;

	camera->setPosition( core::vector3df( 0,100,0) );
	camera->setTarget( core::vector3df(0,0,100) );
	return true;
}

bool Camera::viewNegX( TCamera* camera )
{
	if (!camera)
		return false;

	camera->setPosition( core::vector3df( 0,100,0) );
	camera->setTarget( core::vector3df(0,0,100) );
	return true;
}

bool Camera::viewNegY( TCamera* camera )
{
	if (!camera)
		return false;

	camera->setPosition( core::vector3df( 0,100,0) );
	camera->setTarget( core::vector3df(0,0,100) );
	return true;
}

bool Camera::viewNegZ( TCamera* camera )
{
	if (!camera)
		return false;

	camera->setPosition( core::vector3df( 0,100,0) );
	camera->setTarget( core::vector3df(0,0,100) );
	return true;
}




//
//
//
//
//
//CameraManager::CameraManager( scene::ISceneManager* smgr,
//    gui::IGUIEnvironment* env, gui::IGUIElement* parent, s32 id, const core::recti& rectangle )
//: gui::IGUIElement( gui::EGUIET_ELEMENT, env, parent, id, rectangle), SceneManager(smgr), CurrentCamera(0)
//{
//    const s32 x1 = rectangle.UpperLeftCorner.X;
//    const s32 y1 = rectangle.UpperLeftCorner.Y;
//    const s32 w = rectangle.getWidth();
//    // const s32 h = rectangle.getHeight();
//
//    // gui::IGUIEnvironment* env = env;
//    s32 t = 64;
//    s32 x = x1;
//    s32 y = y1;
//    s32 dx = 2*t;
//    s32 dy = 21;
//    env->addCheckBox( true, core::recti(x,y,x+dx-1,y+dy-1), this, -1, L"AutoUpdate"); x += dx;
//    env->addButton( core::recti(x,y,x+dx-1,y+dy-1), this, -1, L"Update Dialog"); x += dx;
//    env->addButton( core::recti(x,y,x+dx-1,y+dy-1), this, -1, L"Update Camera"); x += dx;
//    y += dy;
//
//    /// index
//    x = x1;
//    dx = t/2;
//    env->addStaticText( L"Nr", core::recti(x,y,x+dx-1,y+dy-1), true, false, this, -1, true); x += dx;
//    dx = t/2;
//    env->addEditBox( L"1", core::recti(x,y,x+dx-1,y+dy-1), true, this, -1); x += dx;
//    dx = t/3;
//    env->addStaticText( L"/", core::recti(x,y,x+dx-1,y+dy-1), true, false, this, -1, true); x += dx;
//    dx = t/2;
//    env->addEditBox( L"1", core::recti(x,y,x+dx-1,y+dy-1), true, this, -1); x += dx;
//    dx = t/3;
//    env->addButton( core::recti(x,y,x+dx-1,y+dy-1), this, -1, L"<"); x += dx;
//    dx = t/3;
//    env->addButton( core::recti(x,y,x+dx-1,y+dy-1), this, -1, L">"); x += dx;
//    dx = 2*t;
//    env->addEditBox( L"Camera Name", core::recti(x,y,x+dx-1,y+dy-1), true, this, -1); x += dx;
//    dx = 2*t;
//    env->addCheckBox( true, core::recti(x,y,x+dx-1,y+dy-1), this, -1, L"Enable"); x += dx;
//    y += dy;
//
//
//    /// position
//    x = x1;
//    new CGUIVector3d<f32>( env, this, -1, core::recti(x,y,x+w-1,y+dy-1), L"Position" );
//    y += dy;
//
//    /// position
//    x = x1;
//    new CGUIVector3d<f32>( env, this, -1, core::recti(x,y,x+w-1,y+dy-1), L"Rotation" );
//    y += dy;
//
//    /// target / eye-vector
//    x = x1;
//    new CGUIVector3d<f32>( env, this, -1, core::recti(x,y,x+w-1,y+dy-1),  L"Target" );
//    y += dy;
//
////    x = x1;
////    dx = t;
////    env->addStaticText( L"Position", core::recti(x,y,x+dx-1,y+dy-1), true, false, this, -1, true); x += dx;
////    dx = dy;
////    env->addStaticText( L"X", core::recti(x,y,x+dx-1,y+dy-1), true, false, this, -1, true); x += dx;
////    dx = dy;
////    env->addCheckBox( true, core::recti(x,y,x+dx-1,y+dy-1), this, -1, L""); x += dx;
////    dx = t;
////    env->addSpinBox( L"0", core::recti(x,y,x+dx-1,y+dy-1), true, this, -1); x += dx;
////    dx = dy;
////    env->addStaticText( L"Y", core::recti(x,y,x+dx-1,y+dy-1), true, false, this, -1, true); x += dx;
////    dx = dy;
////    env->addCheckBox( true, core::recti(x,y,x+dx-1,y+dy-1), this, -1, L""); x += dx;
////    dx = t;
////    env->addSpinBox( L"0", core::recti(x,y,x+dx-1,y+dy-1), true, this, -1); x += dx;
////    dx = dy;
////    env->addStaticText( L"Z", core::recti(x,y,x+dx-1,y+dy-1), true, false, this, -1, true); x += dx;
////    dx = dy;
////    env->addCheckBox( true, core::recti(x,y,x+dx-1,y+dy-1), this, -1, L""); x += dx;
////    dx = t;
////    env->addSpinBox( L"0", core::recti(x,y,x+dx-1,y+dy-1), true, this, -1); x += dx;
////    dx = t;
////    env->addButton( core::recti(x,y,x+dx-1,y+dy-1), this, -1, L"Set");
////    y += dy;
////
////    /// rotation
////    x = x1;
////    dx = t/2;
////    env->addCheckBox( true, core::recti(x,y,x+dx-1,y+dy-1), this, -1, L"X"); x += dx;
////    dx = t;
////    env->addSpinBox( L"0", core::recti(x,y,x+dx-1,y+dy-1), true, this, -1); x += dx;
////    dx = t/2;
////    env->addCheckBox( true, core::recti(x,y,x+dx-1,y+dy-1), this, -1, L"Y"); x += dx;
////    dx = t;
////    env->addSpinBox( L"0", core::recti(x,y,x+dx-1,y+dy-1), true, this, -1); x += dx;
////    dx = t/2;
////    env->addCheckBox( true, core::recti(x,y,x+dx-1,y+dy-1), this, -1, L"Z"); x += dx;
////    dx = t;
////    env->addSpinBox( L"0", core::recti(x,y,x+dx-1,y+dy-1), true, this, -1); x += dx;
////    dx = t/2;
////    env->addButton( core::recti(x,y,x+dx-1,y+dy-1), this, -1, L"Set");
////    y += dy;
//
////        Cameras.reallocate( 32 );
////        Cameras.set_used( 0 );
////        CameraNames.reallocate( 32 );
////        CameraNames.set_used( 0 );
////
////        for ( u32 i = 0; i < 32; i++ )
////        {
////            Cameras.push_back(0);
////            CameraNames.push_back(L"");
////        }
////
////        Cameras.set_used( 0 );
////        CameraNames.set_used( 0 );
////
//        clear();
//
////        addCameraPosZ();
//
////        Camera::createFPS( 0.1, 100.0f );
//}
//
//CameraManager::~CameraManager()
//{
//
//}
//
////! called if an event happened.
//bool CameraManager::OnEvent(const SEvent& event)
//{
//	return IGUIElement::OnEvent(event);
//}
////! draws the element and its children
//void CameraManager::draw()
//{
//	IGUIElement::draw();
//}
//
//bool CameraManager::clear()
//{
//    Cameras.reallocate( 32 );
//    Cameras.set_used( 0 );
//    CameraNames.reallocate( 32 );
//    CameraNames.set_used( 0 );
//
//    for ( u32 i = 0; i < 32; i++ )
//    {
//        Cameras.push_back(0);
//        CameraNames.push_back(L"");
//    }
//
//    Cameras.set_used( 0 );
//    CameraNames.set_used( 0 );
//    return true;
//}
//
//u32 CameraManager::getCameraCount() const
//{
//    return Cameras.size();
//}
//
//scene::ICameraSceneNode* CameraManager::getCurrentCamera( )
//{
//    return CurrentCamera;
//}
//
//bool CameraManager::setCurrentCamera( u32 index )
//{
//    CurrentCamera = getCamera( index );
//    return true;
//}
//
//scene::ICameraSceneNode*
//CameraManager::getCamera( u32 index )
//{
//    if (index-1<Cameras.size())
//    {
//        return Cameras[index-1];
//    }
//    else
//    {
//        return 0;
//    }
//}
//
//scene::ICameraSceneNode*
//CameraManager::getCameraByName( const core::stringw& _compare_name )
//{
//    return 0;
//}
//
//
//
//bool CameraManager::removeCamera( u32 index )
//{
//    return true;
//}
//
//bool CameraManager::setCameraName( u32 index, const core::stringw& name )
//{
//    return true;
//}
//
//core::stringw CameraManager::getCameraName( u32 index ) const
//{
//    return L"";
//}


}


//
//
//
//irr::u32 dbGetFreeCameraIndex()
//{
//	irr::u32 c = Cameras.size();
//	irr::u32 i = 0;
//
//	while (i<c)
//	{
//		if (!irr::Cameras[i])
//		{
//			return i+1;
//		}
//		i++;
//	}
//
//	return 0;
//}
//
//irr::u32 dbGetCameraCount()
//{
//	return irr::Cameras.size();
//}
//
//bool dbExistCamera( irr::u32 cam_id )
//{
//	// get camera array size
//	const irr::u32& count = irr::Cameras.size();
//
//	// valid index ?
//	if ((cam_id == 0) || (cam_id > count))
//		return false;
//
//	// exist camera ?
//	irr::scene::ICameraSceneNode* camera = irr::Cameras[cam_id-1];
//	if (!camera)
//		return false;
//
//	return true;
//}
//
//bool dbDeleteCamera( irr::u32 cam_id )
//{
//	// get camera array size
//	const irr::u32& count = irr::Cameras.size();
//
//	// valid index ?
//	if ((cam_id == 0) || (cam_id > count))
//		return false;
//
//	// exist camera ?
//	irr::scene::ICameraSceneNode* camera = irr::Cameras[cam_id-1];
//	if (!camera)
//		return false;
//
//	// delete and return
//	camera->drop();
//	irr::Cameras[cam_id-1] = 0;
//	return true;
//}
//
//bool dbMakeCamera( irr::scene::ISceneManager* smgr, irr::u32 cam_id )
//{
//	if (!smgr)
//		return false;
//
//	// get camera array size
//	const irr::u32& count = irr::Cameras.size();
//
//	// valid index ?
//	if ((cam_id == 0) || (cam_id > count))
//		return false;
//
//	// exist camera ?
//	irr::scene::ICameraSceneNode* camera = irr::Cameras[cam_id-1];
//	if (camera)
//		return false;
//
//	// create camera scenenode
//	camera = smgr->addCameraSceneNode( smgr->getRootSceneNode(),
//		irr::core::vector3df(0,0,-100), irr::core::vector3df(0,0,0), -1, true);
//
//	// failed ?
//	if (!camera)
//		return 0;
//
//	// store pointer
//	irr::Cameras[cam_id-1] = camera;
//
//	return true;
//}
//
//
//irr::scene::ICameraSceneNode* dbGetCameraByIndex( irr::u32 cam_id )
//{
//	const irr::u32& count = irr::Cameras.size();
//
//	if ((cam_id == 0) || (cam_id > count))
//		return 0;
//
//	return irr::Cameras[cam_id];
//}
//
//bool dbPointCamera( irr::u32 cam_id, irr::f32 x, irr::f32 y, irr::f32 z)
//{
//	irr::scene::ICameraSceneNode* camera = dbGetCameraByIndex( cam_id );
//	if (!camera)
//	{
//		irr::os::Printer::log("dbSetCameraNear()::Could not set Near", irr::ELL_ERROR);
//		return false;
//	}
//
//	return true;
//}
//bool dbSetCameraNear( irr::u32 cam_id, irr::f32 nearValue )
//{
//	irr::scene::ICameraSceneNode* camera = dbGetCameraByIndex( cam_id );
//	if (!camera)
//	{
//		irr::os::Printer::log("dbSetCameraNear()::Could not set Near", irr::ELL_ERROR);
//		return false;
//	}
//
//	return true;
//}
//bool dbSetCameraFar( irr::u32 cam_id, irr::f32 farValue )
//{
//	irr::scene::ICameraSceneNode* camera = dbGetCameraByIndex( cam_id );
//	if (!camera)
//	{
//		irr::os::Printer::log("dbSetCameraNear()::Could not set Near", irr::ELL_ERROR);
//		return false;
//	}
//
//	return true;
//}
//bool dbSetCameraFov( irr::u32 cam_id, irr::f32 fovValue )
//{
//	irr::scene::ICameraSceneNode* camera = dbGetCameraByIndex( cam_id );
//	if (!camera)
//	{
//		irr::os::Printer::log("dbSetCameraNear()::Could not set Near", irr::ELL_ERROR);
//		return false;
//	}
//
//	return true;
//}
//
//bool dbSetCameraFog( irr::u32 cam_id, bool bEnabled, irr::video::E_FOG_TYPE fogType, const irr::video::SColor& fogColor )
//{
//
//    return true;
//}
//
//bool dbSetCameraFogDistance( irr::u32 cam_id, irr::f32 fogNear, irr::f32 fogFar )
//{
//
//}
//
//irr::video::IImage* dbGetCameraImage( irr::u32 cam_id )
//{
//
//}
//
//bool dbSetCurrentCamera( irr::u32 cam_id )
//{
//	irr::scene::ICameraSceneNode* camera = dbGetCameraByIndex( cam_id );
//	if (!camera)
//	{
//		irr::os::Printer::log("dbSetCameraNear()::Could not set Near", irr::ELL_ERROR);
//		return false;
//	}
//
//	return true;
//}
//
//bool dbPositionCamera( irr::u32 cam_id, irr::f32 x, irr::f32 y, irr::f32 z)
//{
//	irr::scene::ICameraSceneNode* camera = dbGetCameraByIndex( cam_id );
//	if (!camera)
//	{
//		irr::os::Printer::log("dbSetCameraNear()::Could not set Near", irr::ELL_ERROR);
//		return false;
//	}
//
//	return true;
//}
//
//bool dbRotateCamera( irr::u32 cam_id, irr::f32 x, irr::f32 y, irr::f32 z)
//{
//	irr::scene::ICameraSceneNode* camera = dbGetCameraByIndex( cam_id );
//	if (!camera)
//	{
//		irr::os::Printer::log("dbSetCameraNear()::Could not set Near", irr::ELL_ERROR);
//		return false;
//	}
//
//	return true;
//}
//
//bool dbRotateCameraX( irr::u32 cam_id, irr::f32 x)
//{
//	irr::scene::ICameraSceneNode* camera = dbGetCameraByIndex( cam_id );
//	if (!camera)
//	{
//		irr::os::Printer::log("dbSetCameraNear()::Could not set Near", irr::ELL_ERROR);
//		return false;
//	}
//
//	return true;
//}
//
//bool dbRotateCameraY( irr::u32 cam_id, irr::f32 y)
//{
//	irr::scene::ICameraSceneNode* camera = dbGetCameraByIndex( cam_id );
//	if (!camera)
//	{
//		irr::os::Printer::log("dbSetCameraNear()::Could not set Near", irr::ELL_ERROR);
//		return false;
//	}
//
//	return true;
//}
//
//bool dbRotateCameraZ( irr::u32 cam_id, irr::f32 z)
//{
//	irr::scene::ICameraSceneNode* camera = dbGetCameraByIndex( cam_id );
//	if (!camera)
//	{
//		irr::os::Printer::log("dbSetCameraNear()::Could not set Near", irr::ELL_ERROR);
//		return false;
//	}
//
//	return true;
//}
//
//
//
//
//irr::f32 dbCameraPositionX( irr::u32 cam_id )
//{
//	return false;
//}
//
//irr::f32 dbCameraPositionY( irr::u32 cam_id )
//{
//	return false;
//}
//
//irr::f32 dbCameraPositionZ( irr::u32 cam_id )
//{
//	return false;
//}
//
//irr::f32 dbCameraRotationX( irr::u32 cam_id )
//{
//	return false;
//}
//
//irr::f32 dbCameraRotationY( irr::u32 cam_id )
//{
//	return false;
//}
//
//irr::f32 dbCameraRotationZ( irr::u32 cam_id )
//{
//	return false;
//}
//
//irr::f32 dbCameraAngleX( irr::u32 cam_id )
//{
//	return false;
//}
//
//irr::f32 dbCameraAngleY( irr::u32 cam_id )
//{
//	return false;
//}
//
//irr::f32 dbCameraAngleZ( irr::u32 cam_id )
//{
//	return false;
//}
//
//
//bool dbMoveCamera( irr::u32 cam_id, irr::f32 speed)
//{
//	irr::scene::ICameraSceneNode* camera = dbGetCameraByIndex( cam_id );
//	if (!camera)
//	{
//		irr::os::Printer::log("dbSetCameraNear()::Could not set Near", irr::ELL_ERROR);
//		return false;
//	}
//
//	return true;
//}
//
//bool dbMoveCameraUp( irr::u32 cam_id, irr::f32 speed)
//{
//	irr::scene::ICameraSceneNode* camera = dbGetCameraByIndex( cam_id );
//	if (!camera)
//	{
//		irr::os::Printer::log("dbSetCameraNear()::Could not set Near", irr::ELL_ERROR);
//		return false;
//	}
//
//	return true;
//}
//
//bool dbMoveCameraX( irr::u32 cam_id, irr::f32 speed)
//{
//	irr::scene::ICameraSceneNode* camera = dbGetCameraByIndex( cam_id );
//	if (!camera)
//	{
//		irr::os::Printer::log("dbSetCameraNear()::Could not set Near", irr::ELL_ERROR);
//		return false;
//	}
//
//	return true;
//}
//
//bool dbMoveCameraY( irr::u32 cam_id, irr::f32 speed)
//{
//	irr::scene::ICameraSceneNode* camera = dbGetCameraByIndex( cam_id );
//	if (!camera)
//	{
//		irr::os::Printer::log("dbSetCameraNear()::Could not set Near", irr::ELL_ERROR);
//		return false;
//	}
//
//	return true;
//}
//
//bool dbMoveCameraZ( irr::u32 cam_id, irr::f32 speed)
//{
//	irr::scene::ICameraSceneNode* camera = dbGetCameraByIndex( cam_id );
//	if (!camera)
//	{
//		irr::os::Printer::log("dbSetCameraNear()::Could not set Near", irr::ELL_ERROR);
//		return false;
//	}
//
//	return true;
//}
//
//bool dbPitchCameraUp( irr::u32 cam_id, irr::f32 speed)
//{
//	irr::scene::ICameraSceneNode* camera = dbGetCameraByIndex( cam_id );
//	if (!camera)
//	{
//		irr::os::Printer::log("dbSetCameraNear()::Could not set Near", irr::ELL_ERROR);
//		return false;
//	}
//
//	return true;
//}
//
//bool dbPitchCameraDown( irr::u32 cam_id, irr::f32 speed)
//{
//	irr::scene::ICameraSceneNode* camera = dbGetCameraByIndex( cam_id );
//	if (!camera)
//	{
//		irr::os::Printer::log("pitch camera down()", irr::ELL_ERROR);
//		return false;
//	}
//
//	return true;
//}
//
//bool dbRollCameraLeft( irr::u32 cam_id, irr::f32 speed)
//{
//	irr::scene::ICameraSceneNode* camera = dbGetCameraByIndex( cam_id );
//	if (!camera)
//	{
//		irr::os::Printer::log("roll camera left()", irr::ELL_ERROR);
//		return false;
//	}
//
//	return true;
//}
//
//bool dbRollCameraRight( irr::u32 cam_id, irr::f32 speed)
//{
//	irr::scene::ICameraSceneNode* camera = dbGetCameraByIndex( cam_id );
//	if (!camera)
//	{
//		irr::os::Printer::log("roll camera right()", irr::ELL_ERROR);
//		return false;
//	}
//
//	return true;
//}
//
//bool dbStrafeCameraLeft( irr::u32 cam_id, irr::f32 speed)
//{
//	irr::scene::ICameraSceneNode* camera = dbGetCameraByIndex( cam_id );
//	if (!camera)
//	{
//		irr::os::Printer::log("strafe camera left()", irr::ELL_ERROR);
//		return false;
//	}
//
//	return true;
//}
//
//bool dbStrafeCameraRight( irr::u32 cam_id, irr::f32 speed)
//{
//	irr::scene::ICameraSceneNode* camera = dbGetCameraByIndex( cam_id );
//	if (!camera)
//	{
//		irr::os::Printer::log("strafe camera right()", irr::ELL_ERROR);
//		return false;
//	}
//
//	return true;
//}
//
//bool dbYawCameraLeft( irr::u32 cam_id, irr::f32 speed)
//{
//	irr::scene::ICameraSceneNode* camera = dbGetCameraByIndex( cam_id );
//	if (!camera)
//	{
//		irr::os::Printer::log("yaw camera left()", irr::ELL_ERROR);
//		return false;
//	}
//
//	return true;
//}
//
//bool dbYawCameraRight( irr::u32 cam_id, irr::f32 speed)
//{
//	irr::scene::ICameraSceneNode* camera = dbGetCameraByIndex( cam_id );
//	if (!camera)
//	{
//		irr::os::Printer::log("yaw camera right()", irr::ELL_ERROR);
//		return false;
//	}
//
//	return true;
//}
