import rclpy
from rclpy.node import Node
from std_msgs.msg import Int32

class ActuatorNode(Node):
    def __init__(self):
        super().__init__('actuator_node')
        self.subscription = self.create_subscription(
            Int32, 'temperature_topic', self.listener_callback, 10)

    def listener_callback(self, msg):
        temp = msg.data
        if temp >= 30:
            self.get_logger().warning(f'ALARM! Temperatura je {temp}C. Palim ventilator (LED ON)!')
        else:
            self.get_logger().info(f'Temperatura je {temp}C. Gasim ventilator (LED OFF).')

def main(args=None):
    rclpy.init(args=args)
    node = ActuatorNode()
    rclpy.spin(node)
    node.destroy_node()
    rclpy.shutdown()

if __name__ == '__main__':
    main()