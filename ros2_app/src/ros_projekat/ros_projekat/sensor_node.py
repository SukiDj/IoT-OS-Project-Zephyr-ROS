import rclpy
from rclpy.node import Node
from rclpy.qos import QoSProfile, ReliabilityPolicy, HistoryPolicy
from ros_projekat_msgs.msg import SensorData
import random

class SensorNode(Node):
    def __init__(self):
        super().__init__('sensor_node')

        self.declare_parameter('publish_period_sec', 2.0)
        self.declare_parameter('start_temp_c', 22.0)
        self.declare_parameter('noise_std', 0.6)

        period = self.get_parameter('publish_period_sec').value
        self.temp = self.get_parameter('start_temp_c').value
        self.noise_std = self.get_parameter('noise_std').value

        qos = QoSProfile(depth=10,
                          reliability=ReliabilityPolicy.RELIABLE,
                          history=HistoryPolicy.KEEP_LAST)

        self.publisher_ = self.create_publisher(SensorData, 'temperature', qos)
        self.timer = self.create_timer(period, self.timer_callback)

    def timer_callback(self):
        # Random-walk simulacija umesto cistog random.randint - realisticnije ponasanje senzora
        drift = random.gauss(0, self.noise_std)
        self.temp = max(10.0, min(45.0, self.temp + drift))

        msg = SensorData()
        msg.header.stamp = self.get_clock().now().to_msg()
        msg.temperature_c = float(self.temp)
        msg.unit = 'C'

        self.publisher_.publish(msg)
        self.get_logger().info(f'Sensor: {msg.temperature_c:.2f} C')

def main(args=None):
    rclpy.init(args=args)
    node = SensorNode()
    rclpy.spin(node)
    node.destroy_node()
    rclpy.shutdown()

if __name__ == '__main__':
    main()