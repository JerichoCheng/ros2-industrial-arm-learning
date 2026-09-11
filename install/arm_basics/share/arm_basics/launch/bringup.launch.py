from launch import LaunchDescription
from launch_ros.actions import Node


def generate_launch_description():
    return LaunchDescription([
        Node(
            package='arm_basics',
            executable='status_pub',
            name='status_pub',
            output='screen',
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