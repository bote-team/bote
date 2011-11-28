#include "StdAfx.h"
#include "BeamSceneNode.h"

namespace irr
{
    namespace scene
    {
        CBeamNode::CBeamNode( ISceneNode* parent, ISceneManager *mgr, s32 id, char* szBeam , char* szBeamFront ) : ISceneNode( parent, mgr, id )
        {
            // Setup the beam material
            material.Wireframe = false;
            material.Lighting = false;
            //material.MaterialType = video::EMT_TRANSPARENT_VERTEX_ALPHA;
            //material.MaterialType = video::EMT_TRANSPARENT_ALPHA_CHANNEL;
            material.MaterialType = video::EMT_TRANSPARENT_ADD_COLOR;
			material.TextureLayer[0].Texture = mgr->getVideoDriver( )->getTexture( szBeam );
            //material.Textures[1] = mgr->getVideoDriver( )->getTexture( szBeam );

            material2.Wireframe = false;
            material2.Lighting = false;
            //material.MaterialType = video::EMT_TRANSPARENT_VERTEX_ALPHA;
            //material.MaterialType = video::EMT_TRANSPARENT_ALPHA_CHANNEL;
            material2.MaterialType = video::EMT_TRANSPARENT_ADD_COLOR;
			material2.TextureLayer[0].Texture = mgr->getVideoDriver( )->getTexture( szBeamFront );

            m_thickness = 1.0f;
        }

        CBeamNode::~CBeamNode(void)
        {

        }

        void CBeamNode::OnRegisterSceneNode(void)
        {
            ISceneNode::OnRegisterSceneNode();
        }

        void CBeamNode::OnAnimate(u32 timeMs)
        {

            if ( IsVisible )
            {
                SceneManager->registerNodeForRendering( this );
            }

            ISceneNode::OnAnimate(timeMs);
        }

        void CBeamNode::setLine(core::vector3df start, core::vector3df end, f32 thickness)
        {
            m_start = start;
            m_end = end;
            m_thickness = thickness;
        }

        void CBeamNode::render(void)
        {
            video::IVideoDriver* driver = SceneManager->getVideoDriver();
            driver->setTransform(irr::video::ETS_WORLD, AbsoluteTransformation);

            core::vector3df direction = m_end-m_start;
            direction.normalize();
            core::vector3df savesatrt = m_start;
            AbsoluteTransformation.transformVect(savesatrt);
            core::vector3df lookdir = savesatrt-SceneManager->getActiveCamera()->getAbsolutePosition();
            if(lookdir.getLength() < 20)
                return;
            lookdir.normalize();

            core::vector3df view(SceneManager->getActiveCamera()->getTarget() - SceneManager->getActiveCamera()->getAbsolutePosition());
            view.normalize();

            if(lookdir.dotProduct(direction)/(lookdir.getLength()*direction.getLength()) < .98f && lookdir.dotProduct(direction)/(lookdir.getLength()*direction.getLength()) > -.98f)
            {
                core::vector3df updown = direction.crossProduct(lookdir);
                updown.normalize();
                core::vector3df normal = direction.crossProduct(updown);
                video::S3DVertex vertices[4];
                u16 indices[] = {0,1,2,1,3,2};

                vertices[0] = video::S3DVertex(m_end-updown*m_thickness*0.5f, normal, video::SColor(255,255,255,255), core::vector2d<f32>(0,0));
                vertices[1] = video::S3DVertex(m_end+updown*m_thickness*0.5f, normal, video::SColor(255,255,255,255), core::vector2d<f32>(0,1));
                vertices[2] = video::S3DVertex(m_start-updown*m_thickness*0.5f, normal, video::SColor(255,255,255,255), core::vector2d<f32>(1,0));
                vertices[3] = video::S3DVertex(m_start+updown*m_thickness*0.5f, normal, video::SColor(255,255,255,255), core::vector2d<f32>(1,1));

                driver->setMaterial(material);
                driver->drawIndexedTriangleList(&vertices[0], 4, &indices[0], 2);
                //driver->draw3DLine(m_start, m_end, video::SColor(255,255,0,0));
            }
            else
            {
                //printf("daw dot\n");

                core::vector3df horizontal = SceneManager->getActiveCamera()->getUpVector().crossProduct(view);
                horizontal.normalize();
                horizontal *= 0.5f * m_thickness;

                core::vector3df vertical = horizontal.crossProduct(view);
                vertical.normalize();
                vertical *= 0.5f * m_thickness;
                view *= -1.0f;

                video::S3DVertex vertices[4];
                u16 indices[] = {0,1,2,1,3,2};

                vertices[0] = video::S3DVertex(m_end - horizontal - vertical, view, video::SColor(255,255,255,255), core::vector2d<f32>(0,0));
                vertices[1] = video::S3DVertex(m_end + horizontal - vertical, view, video::SColor(255,255,255,255), core::vector2d<f32>(0,1));
                vertices[2] = video::S3DVertex(m_end - horizontal + vertical, view, video::SColor(255,255,255,255), core::vector2d<f32>(1,0));
                vertices[3] = video::S3DVertex(m_end + horizontal + vertical, view, video::SColor(255,255,255,255), core::vector2d<f32>(1,1));

                driver->setMaterial(material2);
                driver->drawIndexedTriangleList(&vertices[0], 4, &indices[0], 2);
            }
        }

        const core::aabbox3d<f32>& CBeamNode::getBoundingBox() const
        {
            return Box;
        }

        u32 CBeamNode::getMaterialCount()
        {
            return 2;
        }

        video::SMaterial& CBeamNode::getMaterial(u32 i)
        {
            if(i==0)
                return material;
            else if(i==1)
                return material2;
        }
    }
}

