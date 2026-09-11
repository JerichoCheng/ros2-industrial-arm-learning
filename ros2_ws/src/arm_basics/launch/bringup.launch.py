import os
from ament_index_python.packages import get_package_share_directory
from launch import LaunchDescription
from launch.actions import DeclareLaunchArgument
from launch.substitutions import LaunchConfiguration
from launch_ros.actions import Node


def generate_launch_description():
    pkg_share = get_package_share_directory('arm_basics')
    default_param_file = os.path.join(pkg_share, 'param', 'pub_config.yaml')

    declare_param_file = DeclareLaunchArgument(
        'params_file',
        default_value=default_param_file,
        description='Path to publish_period_sec YAML config for status_pub',
    )
    params_file = LaunchConfiguration('params_file')

    return LaunchDescription([
        declare_param_file,
        Node(
            package='arm_basics',
            executable='status_pub',
            name='status_pub',
            output='screen',
            parameters=[params_file],
        ),
        Node(
            package='arm_basics',
            executable='status_sub',
            name='status_sub',
            output='screen',
        ),
        Node(
            package='arm_basics',
            executable='status_client',
            name='status_client',
            output='screen',
        ),
    ])
