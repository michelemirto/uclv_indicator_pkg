#include "IndicatorLib/Indicator.h"
#include "IndicatorLib/IndicatorCallbacks.h"
#include <ros/ros.h>
#include "read_sensor/tactile_sensor_data.h"
#include "uclv_indicator_pkg/CentroidIndicator.h"
#include "std_srvs/Trigger.h"

std::vector<float> data_read; //vector containing the voltages read from the sensor topic
bool contact = false; // boolean containing true if a contact is detected
int taxel_number; //number of the tactile sensing elements

// Tactile sensor topic callback
void sensor_data_cb(const read_sensor::tactile_sensor_data::ConstPtr &msg)
{
    data_read = msg->tactile_sensor_data;
    
    // if at least one taxel returns a value >= 0.5 a contact is detected
    for (int i =0; i<taxel_number; i++){
        if (data_read[i] >= 0.05)
        {
            contact = true;
            break;
        }
        else
        {
            contact = false;
        }
    }
}

int main(int argc, char **argv)
{
    ros::init(argc, argv, "indicator");
    ros::NodeHandle nh;
    double num_rows, num_cols;
    nh.getParam("/tactile_rows", num_rows);
    nh.getParam("/tactile_columns", num_cols);

    taxel_number = num_rows*num_cols;

    //Istantiate indicator object
    uclv::Indicator indicator(num_rows, num_cols);

    std::string sensor_name;
    std::string sensor_port;
    nh.getParam("/sensor_name", sensor_name);
    nh.getParam("/sensor_port", sensor_port);
    std::string topicName_sensor = "TactileData_dev_" + sensor_port + "_" + sensor_name;
    ros::Subscriber tactile_2_sub = nh.subscribe(topicName_sensor, 1, sensor_data_cb);
    ROS_INFO("Subscribe to tactile data");

    std::string topicName_indicator = "IndicatorData_dev_" + sensor_port + "_" + sensor_name;
    ros::Publisher indicator_pub = nh.advertise<uclv_indicator_pkg::CentroidIndicator>(topicName_indicator, 1);
   
    // advertise service to reset the indicator
    ros::ServiceServer service = nh.advertiseService<std_srvs::Trigger::Request, std_srvs::Trigger::Response>("zero_indicator", boost::bind(uclv::zero_cb, _1, _2, indicator));

    // fill data_read
    double rate = 100;
    ros::Rate loop_rate_0(rate);

    int i = 0;
    while (ros::ok() && i<50)
    {
        loop_rate_0.sleep();
        ros::spinOnce();
        i++;
    }
    //

    rate = 500;
    ros::Rate loop_rate(rate);

    //first reset the indicator
    indicator.compute_indicator(data_read);
    indicator.zero_indicator();

    uclv_indicator_pkg::CentroidIndicator indicator_msg;

    // compute indicator in a while loop and publish on topic
    while(ros::ok())
    {
        ros::spinOnce();
        indicator.compute_indicator(data_read);
        indicator_msg.header.stamp = ros::Time::now();

        // if a contact is detected publish the indicator value, otherwise publish 0
        if (contact)
        {
            indicator_msg.Ix = indicator.Ix();
            indicator_msg.Iy = indicator.Iy();
            indicator_msg.I = indicator.I();
        }
        else
        {
            indicator_msg.Ix = 0.0;
            indicator_msg.Iy = 0.0;
            indicator_msg.I = 0.0;
        }    

        indicator_pub.publish(indicator_msg);

        loop_rate.sleep();
    }
    return 0;
}