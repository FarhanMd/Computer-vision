/****************************************************************************
*                                                                           *
*  OpenNI 1.x Alpha                                                         *
*  Copyright (C) 2011 PrimeSense Ltd.                                       *
*                                                                           *
*  This file is part of OpenNI.                                             *
*                                                                           *
*  OpenNI is free software: you can redistribute it and/or modify           *
*  it under the terms of the GNU Lesser General Public License as published *
*  by the Free Software Foundation, either version 3 of the License, or     *
*  (at your option) any later version.                                      *
*                                                                           *
*  OpenNI is distributed in the hope that it will be useful,                *
*  but WITHOUT ANY WARRANTY; without even the implied warranty of           *
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the             *
*  GNU Lesser General Public License for more details.                      *
*                                                                           *
*  You should have received a copy of the GNU Lesser General Public License *
*  along with OpenNI. If not, see <http://www.gnu.org/licenses/>.           *
*                                                                           *
****************************************************************************/
//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include <XnCppWrapper.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h> 

//---------------------------------------------------------------------------
// Defines
//---------------------------------------------------------------------------
#define SAMPLE_XML_PATH "../../../../Data/SamplesConfig.xml"
#define SAMPLE_XML_PATH_LOCAL "SamplesConfig.xml"

//---------------------------------------------------------------------------
// Globals
//---------------------------------------------------------------------------
xn::Context g_Context;
xn::ScriptNode g_scriptNode;
xn::DepthGenerator g_DepthGenerator;
xn::UserGenerator g_UserGenerator;

XnBool g_bNeedPose = FALSE;
XnChar g_strPose[20] = "";

#define MAX_NUM_USERS 15
//---------------------------------------------------------------------------
// Code
//---------------------------------------------------------------------------



XnBool fileExists(const char *fn)
{
	XnBool exists;
	xnOSDoesFileExist(fn, &exists);
	return exists;
}

// Callback: New user was detected
void XN_CALLBACK_TYPE User_NewUser(xn::UserGenerator& /*generator*/, XnUserID nId, void* /*pCookie*/)
{
    XnUInt32 epochTime = 0;
    xnOSGetEpochTime(&epochTime);
    printf("%d New User %d\n", epochTime, nId);
    // New user found
    if (g_bNeedPose)
    {
        g_UserGenerator.GetPoseDetectionCap().StartPoseDetection(g_strPose, nId);
    }
    else
    {
        g_UserGenerator.GetSkeletonCap().RequestCalibration(nId, TRUE);
    }
}
// Callback: An existing user was lost
void XN_CALLBACK_TYPE User_LostUser(xn::UserGenerator& /*generator*/, XnUserID nId, void* /*pCookie*/)
{
    XnUInt32 epochTime = 0;
    xnOSGetEpochTime(&epochTime);
    printf("%d Lost user %d\n", epochTime, nId);	
}
// Callback: Detected a pose
void XN_CALLBACK_TYPE UserPose_PoseDetected(xn::PoseDetectionCapability& /*capability*/, const XnChar* strPose, XnUserID nId, void* /*pCookie*/)
{
    XnUInt32 epochTime = 0;
    xnOSGetEpochTime(&epochTime);
    printf("%d Pose %s detected for user %d\n", epochTime, strPose, nId);
    g_UserGenerator.GetPoseDetectionCap().StopPoseDetection(nId);
    g_UserGenerator.GetSkeletonCap().RequestCalibration(nId, TRUE);
}
// Callback: Started calibration
void XN_CALLBACK_TYPE UserCalibration_CalibrationStart(xn::SkeletonCapability& /*capability*/, XnUserID nId, void* /*pCookie*/)
{
    XnUInt32 epochTime = 0;
    xnOSGetEpochTime(&epochTime);
    printf("%d Calibration started for user %d\n", epochTime, nId);
}

void XN_CALLBACK_TYPE UserCalibration_CalibrationComplete(xn::SkeletonCapability& /*capability*/, XnUserID nId, XnCalibrationStatus eStatus, void* /*pCookie*/)
{
    XnUInt32 epochTime = 0;
    xnOSGetEpochTime(&epochTime);
    if (eStatus == XN_CALIBRATION_STATUS_OK)
    {
        // Calibration succeeded
        printf("%d Calibration complete, start tracking user %d\n", epochTime, nId);		
        g_UserGenerator.GetSkeletonCap().StartTracking(nId);
    }
    else
    {
        // Calibration failed
        printf("%d Calibration failed for user %d\n", epochTime, nId);
        if(eStatus==XN_CALIBRATION_STATUS_MANUAL_ABORT)
        {
            printf("Manual abort occured, stop attempting to calibrate!");
            return;
        }
        if (g_bNeedPose)
        {
            g_UserGenerator.GetPoseDetectionCap().StartPoseDetection(g_strPose, nId);
        }
        else
        {
            g_UserGenerator.GetSkeletonCap().RequestCalibration(nId, TRUE);
        }
    }
}


#define CHECK_RC(nRetVal, what)					    \
    if (nRetVal != XN_STATUS_OK)				    \
{								    \
    printf("%s failed: %s\n", what, xnGetStatusString(nRetVal));    \
    return nRetVal;						    \
}

int main(int argc,char *argv[])
{
    XnStatus nRetVal = XN_STATUS_OK;
    xn::EnumerationErrors errors;
	int sockfd = 0, n = 0,alpha;
    	char recvBuff[1024];
   	struct sockaddr_in serv_addr; 
	char buff[10];
	int cmd;
	
    if(argc != 2)
    {
        printf("\n Usage: %s <ip of server> \n",argv[0]);
        return 1;
    } 

    memset(recvBuff, '0',sizeof(recvBuff));
    if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("\n Error : Could not create socket \n");
        return 1;
    } 

    memset(&serv_addr, '0', sizeof(serv_addr)); 

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(5000); 

    if(inet_pton(AF_INET, argv[1], &serv_addr.sin_addr)<=0)
    {
        printf("\n inet_pton error occured\n");
        return 1;
    } 

    if( connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
       printf("\n Error : Connect Failed \n");
       return 1;
    } 

    const char *fn = NULL;
    if    (fileExists(SAMPLE_XML_PATH)) fn = SAMPLE_XML_PATH;
    else if (fileExists(SAMPLE_XML_PATH_LOCAL)) fn = SAMPLE_XML_PATH_LOCAL;
    else {
        printf("Could not find '%s' nor '%s'. Aborting.\n" , SAMPLE_XML_PATH, SAMPLE_XML_PATH_LOCAL);
        return XN_STATUS_ERROR;
    }
    printf("Reading config from: '%s'\n", fn);

    nRetVal = g_Context.InitFromXmlFile(fn, g_scriptNode, &errors);
    if (nRetVal == XN_STATUS_NO_NODE_PRESENT)
    {
        XnChar strError[1024];
        errors.ToString(strError, 1024);
        printf("%s\n", strError);
        return (nRetVal);
    }
    else if (nRetVal != XN_STATUS_OK)
    {
        printf("Open failed: %s\n", xnGetStatusString(nRetVal));
        return (nRetVal);
    }

    nRetVal = g_Context.FindExistingNode(XN_NODE_TYPE_DEPTH, g_DepthGenerator);
    CHECK_RC(nRetVal,"No depth");

    nRetVal = g_Context.FindExistingNode(XN_NODE_TYPE_USER, g_UserGenerator);
    if (nRetVal != XN_STATUS_OK)
    {
        nRetVal = g_UserGenerator.Create(g_Context);
        CHECK_RC(nRetVal, "Find user generator");
    }

    XnCallbackHandle hUserCallbacks, hCalibrationStart, hCalibrationComplete, hPoseDetected;
    if (!g_UserGenerator.IsCapabilitySupported(XN_CAPABILITY_SKELETON))
    {
        printf("Supplied user generator doesn't support skeleton\n");
        return 1;
    }
    nRetVal = g_UserGenerator.RegisterUserCallbacks(User_NewUser, User_LostUser, NULL, hUserCallbacks);
    CHECK_RC(nRetVal, "Register to user callbacks");
    nRetVal = g_UserGenerator.GetSkeletonCap().RegisterToCalibrationStart(UserCalibration_CalibrationStart, NULL, hCalibrationStart);
    CHECK_RC(nRetVal, "Register to calibration start");
    nRetVal = g_UserGenerator.GetSkeletonCap().RegisterToCalibrationComplete(UserCalibration_CalibrationComplete, NULL, hCalibrationComplete);
    CHECK_RC(nRetVal, "Register to calibration complete");

    if (g_UserGenerator.GetSkeletonCap().NeedPoseForCalibration())
    {
        g_bNeedPose = TRUE;
        if (!g_UserGenerator.IsCapabilitySupported(XN_CAPABILITY_POSE_DETECTION))
        {
            printf("Pose required, but not supported\n");
            return 1;
        }
        nRetVal = g_UserGenerator.GetPoseDetectionCap().RegisterToPoseDetected(UserPose_PoseDetected, NULL, hPoseDetected);
        CHECK_RC(nRetVal, "Register to Pose Detected");
        g_UserGenerator.GetSkeletonCap().GetCalibrationPose(g_strPose);
    }

    g_UserGenerator.GetSkeletonCap().SetSkeletonProfile(XN_SKEL_PROFILE_ALL);

    nRetVal = g_Context.StartGeneratingAll();
    CHECK_RC(nRetVal, "StartGenerating");

    XnUserID aUsers[MAX_NUM_USERS];
    XnUInt16 nUsers;
    XnSkeletonJointTransformation leftshoulderJoint,leftelbowJoint,leftkneeJoint;
    XnSkeletonJointTransformation rightshoulderJoint,rightelbowJoint,rightkneeJoint,rightwristJoint;
    printf("Starting to run\n");
    if(g_bNeedPose)
    {
        printf("Assume calibration pose\n");
    }

	while (!xnOSWasKeyboardHit())
    {    
	 sleep(1.2);
        g_Context.WaitOneUpdateAll(g_UserGenerator);
        // print the torso information for the first user already tracking
        nUsers=MAX_NUM_USERS;
        g_UserGenerator.GetUsers(aUsers, nUsers);
        for(XnUInt16 i=0; i<nUsers; i++)
        {
            if(g_UserGenerator.GetSkeletonCap().IsTracking(aUsers[i])==FALSE)
                continue;

            g_UserGenerator.GetSkeletonCap().GetSkeletonJoint(aUsers[i],XN_SKEL_LEFT_SHOULDER ,leftshoulderJoint);
            g_UserGenerator.GetSkeletonCap().GetSkeletonJoint(aUsers[i],XN_SKEL_RIGHT_SHOULDER,rightshoulderJoint);
            g_UserGenerator.GetSkeletonCap().GetSkeletonJoint(aUsers[i],XN_SKEL_LEFT_ELBOW,leftelbowJoint);
            g_UserGenerator.GetSkeletonCap().GetSkeletonJoint(aUsers[i],XN_SKEL_RIGHT_ELBOW,rightelbowJoint);
 	    g_UserGenerator.GetSkeletonCap().GetSkeletonJoint(aUsers[i],XN_SKEL_LEFT_KNEE,leftkneeJoint);
	    g_UserGenerator.GetSkeletonCap().GetSkeletonJoint(aUsers[i],XN_SKEL_RIGHT_KNEE,rightkneeJoint);
	    g_UserGenerator.GetSkeletonCap().GetSkeletonJoint(aUsers[i],XN_SKEL_RIGHT_WRIST,rightwristJoint);

                printf("user %d: leftshoulder at Z-axis(%6.2f)\n",aUsers[i],leftshoulderJoint.position.position.Z);
                printf("user %d: rightshoulder at Z-axis (%6.2f)\n",aUsers[i],rightshoulderJoint.position.position.Z);
                printf("user %d: leftshoulder at Y-axis(%6.2f)\n",aUsers[i],leftshoulderJoint.position.position.Y);
                printf("user %d: rightshoulder at Y-axis (%6.2f)\n",aUsers[i],rightshoulderJoint.position.position.Y);
         	printf("user %d: leftshoulder at X-axis(%6.2f)\n",aUsers[i],leftshoulderJoint.position.position.X);
                printf("user %d: rightshoulder at X-axis (%6.2f)\n",aUsers[i],rightshoulderJoint.position.position.X);
                printf("user %d: leftelbow at Y- axis(%6.2f)\n",aUsers[i],leftelbowJoint.position.position.Y);
                printf("user %d: rightelbow at Y- axis(%6.2f)\n",aUsers[i],rightelbowJoint.position.position.Y);
        	printf("user %d: leftelbow at X- axis(%6.2f)\n",aUsers[i],leftelbowJoint.position.position.X);
                printf("user %d: rightelbow at X- axis(%6.2f)\n",aUsers[i],rightelbowJoint.position.position.X);
		printf("user %d: leftknee at Z-axis(%6.2f)\n",aUsers[i],leftkneeJoint.position.position.Z);
		printf("user %d: rightknee at z-axis(%6.2f)\n",aUsers[i],rightkneeJoint.position.position.Z);
                

               if((leftshoulderJoint.position.position.X)>(leftelbowJoint.position.position.X))
		{        
                         
			// sleep(1);
			 buff[0]='5';
			 buff[1]=0;
			alpha= write(sockfd, buff,1);
                        
        		//if (alpha == -1) break ;
                       // printf("the value of alpha is :%d",alpha);
		}
               if((rightshoulderJoint.position.position.X)<(rightelbowJoint.position.position.X))
		{       
                       
			// sleep(1);
			buff[0]='6';
			 buff[1]=0;
			alpha= write(sockfd, buff,1);
                      
		}
               if((leftelbowJoint.position.position.Y)>(leftshoulderJoint.position.position.Y))
		{      
                        
			//sleep(1);
			buff[0]='2';
			 buff[1]=0;
			alpha= write(sockfd, buff,1);
                        
		}
		if((leftelbowJoint.position.position.Y)<(leftshoulderJoint.position.position.Y))
		{      
                        
			//sleep(1);
			buff[0]='4';
			 buff[1]=0;
			alpha= write(sockfd, buff,1);
                        
		}
               
              if((rightelbowJoint.position.position.Y)>(rightshoulderJoint.position.position.Y))   
		{
                         
			//sleep(1);
			buff[0]='1';
			buff[1]=0;
			alpha= write(sockfd, buff,1);
                       
		}
		if((rightelbowJoint.position.position.Y)<(rightshoulderJoint.position.position.Y))   
		{
                         
			//sleep(1);
			buff[0]='3';
			buff[1]=0;
			alpha= write(sockfd, buff,1);
                       
		}
		if((rightkneeJoint.position.position.Z)<(leftkneeJoint.position.position.Z))   
		{
                         
			//sleep(1);
			buff[0]='7';
			buff[1]=0;
			alpha= write(sockfd, buff,1);
                       
		}
		if((rightkneeJoint.position.position.Z)>(leftkneeJoint.position.position.Z)) 
		{
                         
			//sleep(1);
			buff[0]='8';
			buff[1]=0;
			alpha= write(sockfd, buff,1);
                       
		}
               
        }
        
    }
    g_scriptNode.Release();
    g_DepthGenerator.Release();
    g_UserGenerator.Release();
    g_Context.Release();
	close(sockfd);
	return 0;
 
    exit(0);

}
