//
// Copyright (c) 2019 Jimmy Lord http://www.flatheadgames.com
//
#ifndef __OpenCVNodesFace_H__
#define __OpenCVNodesFace_H__

#include "Utility/Helpers.h"
#include "Utility/VectorTypes.h"
#include "Libraries/Engine/MyEngine/SourceEditor/PlatformSpecific/FileOpenDialog.h"
#include "OpenCVNodes_Base.h"

class ComponentBase;

// OpenCV node types.
class OpenCVNode_Face_Detect;

//====================================================================================================
// OpenCVNode_Face_Detect
//====================================================================================================

class OpenCVNode_Face_Detect : public OpenCVBaseNode
{
protected:
    cv::Mat m_Image;
    TextureDefinition* m_pTexture;
    cv::CascadeClassifier m_FaceClassifier;
    cv::CascadeClassifier m_EyesClassifier;
    cv::CascadeClassifier m_MouthClassifier;
    cv::CascadeClassifier m_NoseClassifier;

public:
    OpenCVNode_Face_Detect(OpenCVNodeGraph* pNodeGraph, OpenCVNodeGraph::NodeID id, const char* name, const Vector2& pos)
        : OpenCVBaseNode( pNodeGraph, id, name, pos, 1, 1 )
    {
        m_pTexture = nullptr;
        //VSNAddVar( &m_VariablesList, "Float", ComponentVariableType_Float, MyOffsetOf( this, &this->m_Float ), true, true, "", nullptr, nullptr, nullptr );
        
    }

    ~OpenCVNode_Face_Detect()
    {
        SAFE_RELEASE( m_pTexture );
    }

    const char* GetType() { return "Face_Detect"; }
    //virtual uint32 EmitLua(char* string, uint32 offset, uint32 bytesAllocated, uint32 tabDepth) override;

    void LoadClassifiers()
    {
        // Load classifiers from "Data/OpenCVHaarCascades" directory  
        m_FaceClassifier.load( "Data/OpenCVHaarCascades/haarcascade_frontalface_default.xml" ) ;
        m_EyesClassifier.load( "Data/OpenCVHaarCascades/haarcascade_eye_tree_eyeglasses.xml" );
        //m_MouthClassifier.load( "Data/OpenCVHaarCascades/haarcascade_smile.xml" );
        m_MouthClassifier.load( "Data/OtherHaarCascades/haarcascade_mcs_mouth.xml" );
        m_NoseClassifier.load( "Data/OtherHaarCascades/haarcascade_mcs_nose.xml" );
    }

    virtual void DrawTitle() override
    {
        if( m_Expanded )
            OpenCVBaseNode::DrawTitle();
        else
            ImGui::Text( "%s", m_Name );
    }

    virtual bool DrawContents() override
    {
        bool modified = OpenCVBaseNode::DrawContents();

        if( m_FaceClassifier.empty() == true )
        {
            ImGui::Text( "Haar cascades not loaded." );
            if( ImGui::Button( "Load" ) )
            {
                LoadClassifiers();
            }
        }

        DisplayOpenCVMatAndTexture( &m_Image, m_pTexture, m_pNodeGraph->GetImageWidth(), m_pNodeGraph->GetHoverPixelsToShow() );

        return modified;
    }

    virtual bool Trigger(MyEvent* pEvent, bool recursive) override
    {
        if( m_FaceClassifier.empty() == true )
            return false;

        //OpenCVBaseNode::Trigger( pEvent );

        // Get Image from input node.
        cv::Mat* pImage = GetInputImage( 0 );

        if( pImage )
        {
            cv::Mat imageGray;
            cvtColor( *pImage, imageGray, cv::COLOR_BGR2GRAY );
            equalizeHist( imageGray, imageGray );

            // Make a copy of the source image, we'll draw shapes into this one.
            pImage->copyTo( m_Image );

            // Find the face.
            std::vector<cv::Rect> faceRects;
            {
                m_FaceClassifier.detectMultiScale( imageGray, faceRects );

                // Draw a rectangle around the face and find more features.
                for( uint32 i=0; i<faceRects.size(); i++ )
                {
                    cv::Rect faceRect = faceRects[i];

                    // Draw a blue rectangle around the face.
                    rectangle( m_Image, faceRect, cv::Scalar( 255, 0, 0 ), 4 );

                    // Find the eyes.
                    std::vector<cv::Rect> eyeRects;
                    if( faceRects.size() > 0 )
                    {
                        // Start with just the face region.
                        cv::Mat imageFace = imageGray( faceRect );

                        // Find the eyes.
                        m_EyesClassifier.detectMultiScale( imageFace, eyeRects );

                        // Draw blue circles around the eyes.
                        for( uint32 j=0; j<eyeRects.size(); j++ )
                        {
                            cv::Rect eyeRect = eyeRects[j];
                            cv::Point center( faceRect.x + eyeRect.x + eyeRect.width/2,
                                              faceRect.y + eyeRect.y + eyeRect.height/2 );
                            int radius = cvRound( (eyeRect.width + eyeRect.height) * 0.25 );
                            circle( m_Image, center, radius, cv::Scalar( 255, 0, 0 ), 4 );
                        }
                    }

                    // Find the nose.
                    std::vector<cv::Rect> noseRects;
                    cv::Rect faceBelowEyesRect = faceRect;
                    if( eyeRects.size() > 0 )
                    {
                        // Start with just the face region below the eyes.
                        faceBelowEyesRect.y = faceRect.y + eyeRects[0].y + eyeRects[0].height/2;
                        faceBelowEyesRect.height -= faceBelowEyesRect.y - faceRect.y;
                        cv::Mat imageFace = imageGray( faceBelowEyesRect );

                        // Find the nose.
                        m_NoseClassifier.detectMultiScale( imageFace, noseRects );

                        // Draw red circles around the nose.
                        for( uint32 j=0; j<noseRects.size(); j++ )
                        {
                            cv::Rect noseRect = noseRects[j];
                            cv::Point center( faceBelowEyesRect.x + noseRect.x + noseRect.width/2,
                                                faceBelowEyesRect.y + noseRect.y + noseRect.height/2 );
                            int radius = cvRound( (noseRect.width + noseRect.height) * 0.25 );
                            circle( m_Image, center, radius, cv::Scalar( 0, 0, 255 ), 4 );
                        }
                    }

                    // Find the mouth.
                    std::vector<cv::Rect> mouthRects;
                    if( noseRects.size() > 0 )
                    {
                        // Start with just the face region below the nose.
                        cv::Rect faceBelowNoseRect = faceBelowEyesRect;
                        faceBelowNoseRect.y = faceBelowEyesRect.y + noseRects[0].y + noseRects[0].height/2;
                        faceBelowNoseRect.height -= faceBelowNoseRect.y - faceBelowEyesRect.y;
                        cv::Mat imageFace = imageGray( faceBelowNoseRect );

                        // Find the mouth.
                        m_MouthClassifier.detectMultiScale( imageFace, mouthRects );

                        // Draw green circles around the mouth.
                        for( uint32 j=0; j<mouthRects.size(); j++ )
                        {
                            cv::Rect mouthRect = mouthRects[j];
                            cv::Point center( faceBelowNoseRect.x + mouthRect.x + mouthRect.width/2,
                                              faceBelowNoseRect.y + mouthRect.y + mouthRect.height/2 );
                            int radius = cvRound( (mouthRect.width + mouthRect.height) * 0.25 );
                            circle( m_Image, center, radius, cv::Scalar( 0, 255, 0 ), 4 );
                        }
                    }
                }
            }

            m_pTexture = CreateOrUpdateTextureDefinitionFromOpenCVMat( &m_Image, m_pTexture );

            // Trigger the output nodes.
            TriggerOutputNodes( pEvent, recursive );
        }

        return false;
    }

    virtual cJSON* ExportAsJSONObject() override
    {
        cJSON* jNode = OpenCVBaseNode::ExportAsJSONObject();
        return jNode;
    }

    virtual void ImportFromJSONObject(cJSON* jNode) override
    {
        MyNode::ImportFromJSONObject( jNode );
    }

    virtual cv::Mat* GetValueMat() override { return &m_Image; }
};

#endif //__OpenCVNodesFace_H__
