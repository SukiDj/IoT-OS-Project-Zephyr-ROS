import rclpy
from rclpy.node import Node
from std_msgs.msg import Int32
import random

class SensorNode(Node):
    def __init__(self):
        super().__init__('sensor_node')
        self.publisher_ = self.create_publisher(Int32, 'temperature_topic', 10)
        self.timer = self.create_timer(2.0, self.timer_callback)

    def timer_callback(self):
        msg = Int32()
        msg.data = random.randint(20, 35)
        self.get_logger().info(f'Senzor salje: {msg.data} C')
        self.publisher_.publish(msg)

def main(args=None):
    rclpy.init(args=args)
    node = SensorNode()
    rclpy.spin(node)
    node.destroy_node()
    rclpy.shutdown()

if __name__ == '__main__':
    main()