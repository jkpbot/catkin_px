#include <ros/ros.h>
#include <geometry_msgs/PoseStamped.h>
#include <mavros_msgs/CommandBool.h>
#include <mavros_msgs/State.h>
#include <mavros_msgs/SetMode.h>
#include "std_msgs/String.h"
#include <sstream>
#include <math.h>


int cel=0;



geometry_msgs::PoseStamped pose;




mavros_msgs::State status;

mavros_msgs::State current_state;
// float radians(float radians) 
// {
//     return radians * (180/M_PI);
// }
// float degrees(float degrees) 
// {
//     return degrees * (M_PI/ 180);
// }

// float bearing(float lat,float lon,float lat2,float lon2)
// {
// //lat = your current gps latitude.
// //lon = your current gps longitude.
// //lat2 = your destiny gps latitude.
// //lon2 = your destiny gps longitude.

//     float teta1 = radians(lat);
//     float teta2 = radians(lat2);

//     float delta1 = radians(lat2-lat);
//     float delta2 = radians(lon2-lon);

//     //==================Heading Formula Calculation================//

//     float y = sin(delta2) * cos(teta2);
//     float x = cos(teta1)*sin(teta2) - sin(teta1)*cos(teta2)*cos(delta2);


//     float brng = atan2(y,x);
//     brng = degrees(brng);// radians to degrees
//     brng = ( ((int)brng + 360) % 360 ); 


//     return brng;
//   }



void state_cb(const mavros_msgs::State::ConstPtr& msg)
{
    
    std_msgs::String pub_str;
    std::stringstream ss;
    ss<<"slysze z statea "<<msg->armed<<std::endl;
    pub_str.data=ss.str();
    std::cout<<pub_str.data.c_str()<<std::endl;
    current_state = *msg;
}


void chatterCallback(const geometry_msgs::PoseStamped::ConstPtr& msg)
    {
      
    std_msgs::String pub_str;
    std::stringstream ss;
    ss<<"slysze "<<msg->pose.position.x<<"  ,  "<<msg->pose.position.y<<"  ,  "<<msg->pose.position.z<<std::endl;
    pub_str.data=ss.str();
    std::cout<<pub_str.data.c_str()<<std::endl;
   
    switch (cel)
     {
      case 0: 
        
            cel++;
         break;
      
      case 1: 
      if((msg->pose.position.z > pose.pose.position.z-1 &&  msg->pose.position.z < pose.pose.position.z+1) &&(msg->pose.position.x > pose.pose.position.x-1 &&  msg->pose.position.x < pose.pose.position.x+1) && (msg->pose.position.y > pose.pose.position.y-1 &&  msg->pose.position.y < pose.pose.position.y+1))
        {
            pose.pose.position.x = 0;
            pose.pose.position.y = 0;
            pose.pose.position.z = 5;
           
            cel++;
        }break;
     
      
      case 2:
      if((msg->pose.position.z > pose.pose.position.z-1 &&  msg->pose.position.z < pose.pose.position.z+1) &&(msg->pose.position.x > pose.pose.position.x-1 &&  msg->pose.position.x < pose.pose.position.x+1) && (msg->pose.position.y > pose.pose.position.y-1 &&  msg->pose.position.y < pose.pose.position.y+1))
        {
        pose.pose.position.x = 30;
        pose.pose.position.y = 0;
        pose.pose.position.z = 5;
        
         cel++;
        }break;
        
      case 3:
      if((msg->pose.position.z > pose.pose.position.z-1 &&  msg->pose.position.z < pose.pose.position.z+1) &&(msg->pose.position.x > pose.pose.position.x-1 &&  msg->pose.position.x < pose.pose.position.x+1) && (msg->pose.position.y > pose.pose.position.y-1 &&  msg->pose.position.y < pose.pose.position.y+1))
        {
        pose.pose.position.x = 30;
        pose.pose.position.y = 30;
        pose.pose.position.z = 5;
      
         cel++;
        }break;
      
      case 4:
      if((msg->pose.position.z > pose.pose.position.z-1 &&  msg->pose.position.z < pose.pose.position.z+1) &&(msg->pose.position.x > pose.pose.position.x-1 &&  msg->pose.position.x < pose.pose.position.x+1) && (msg->pose.position.y > pose.pose.position.y-1 &&  msg->pose.position.y < pose.pose.position.y+1))
        {
        pose.pose.position.x = 0;
        pose.pose.position.y = 30;
        pose.pose.position.z = 5;
        
        cel=0;
        }break;
       } 
    }


int main(int argc, char **argv)
{
    ros::init(argc, argv, "kontrolalotow2");
    ros::NodeHandle nh;
    ros::NodeHandle n;
    ros::Subscriber state_sub = nh.subscribe<mavros_msgs::State> ("uav0/mavros/state", 10, state_cb);
    
    ros::Subscriber pozycja=    n.subscribe<geometry_msgs::PoseStamped>("uav0/mavros/local_position/pose",1000,chatterCallback);

    ros::Publisher local_pos_pub = nh.advertise<geometry_msgs::PoseStamped>("uav0/mavros/setpoint_position/local", 10);
    
    ros::Publisher statuser = n.advertise<mavros_msgs::State>("uav0/mavros/state", 10,state_cb);
    
    ros::ServiceClient set_mode_client = nh.serviceClient<mavros_msgs::SetMode>("uav0/mavros/set_mode");
      
   ros::ServiceClient arming_client = nh.serviceClient<mavros_msgs::CommandBool>("uav0/mavros/cmd/arming");
        

   
    ros::Rate rate(20.0);

    ros::Time last_request = ros::Time::now();
    
    while(ros::ok() && !current_state.connected){
        ros::spinOnce();
        rate.sleep();
    }
    for(int i = 100; ros::ok() && i > 0; --i){
        local_pos_pub.publish(pose);
        ros::spinOnce();
        rate.sleep();
    }

  


    mavros_msgs::SetMode offb_set_mode;
    offb_set_mode.request.base_mode = 0;
    offb_set_mode.request.custom_mode = "OFFBOARD";

    mavros_msgs::CommandBool arm_cmd;
    arm_cmd.request.value = true;
    
    while(ros::ok())
    {
        if( current_state.mode != "OFFBOARD" &&
            (ros::Time::now() - last_request > ros::Duration(5.0))){
            if( set_mode_client.call(offb_set_mode) &&
                offb_set_mode.response.mode_sent){
                ROS_INFO("Offboard enabled");
            }
            last_request = ros::Time::now();
        } else {
            if( !current_state.armed &&
                (ros::Time::now() - last_request > ros::Duration(5.0))){
                if( arming_client.call(arm_cmd) &&
                    arm_cmd.response.success){
                    ROS_INFO("Vehicle armed");
                }
                last_request = ros::Time::now();
            }
        }
        std::cout<<"nadaje"<<std::endl;
        local_pos_pub.publish(pose);
        statuser.publish(status);
        ros::spinOnce();
        rate.sleep();
    }
        
        
         
      
    

    return 0;
}