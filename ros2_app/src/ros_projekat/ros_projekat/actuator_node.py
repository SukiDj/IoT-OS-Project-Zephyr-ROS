import rclpy
from rclpy.node import Node
from ros_projekat_msgs.msg import SensorData
from ros_projekat_msgs.srv import SetThreshold

class ActuatorNode(Node):
    def __init__(self):
        super().__init__('actuator_node')

        self.declare_parameter('threshold_c', 30.0)
        self.threshold = self.get_parameter('threshold_c').value
        self.fan_on = False

        self.subscription = self.create_subscription(
            SensorData, 'temperature', self.listener_callback, 10)

        self.srv = self.create_service(
            SetThreshold, 'set_threshold', self.set_threshold_callback)

    def listener_callback(self, msg: SensorData):
        should_be_on = msg.temperature_c >= self.threshold
        if should_be_on != self.fan_on:
            self.fan_on = should_be_on
            state = 'ON' if self.fan_on else 'OFF'
            self.get_logger().warning(
                f'AKTUACIJA: {msg.temperature_c:.1f}C (prag {self.threshold:.1f}C) -> VENTILATOR {state}')
        else:
            self.get_logger().info(f'{msg.temperature_c:.1f}C, ventilator ostaje {"ON" if self.fan_on else "OFF"}')

    def set_threshold_callback(self, request, response):
        old = self.threshold
        self.threshold = request.new_threshold
        response.success = True
        response.old_threshold = old
        self.get_logger().info(f'Prag promenjen: {old:.1f}C -> {self.threshold:.1f}C')
        return response

def main(args=None):
    rclpy.init(args=args)
    node = ActuatorNode()
    rclpy.spin(node)
    node.destroy_node()
    rclpy.shutdown()

if __name__ == '__main__':
    main()