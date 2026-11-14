"""AXP2101 ALDO power and battery status for boards that wire this PMIC."""
import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import i2c, sensor, binary_sensor
from esphome.const import CONF_ID

DEPENDENCIES = ["i2c"]
AUTO_LOAD = ["sensor", "binary_sensor"]
MULTI_CONF = True
ns = cg.esphome_ns.namespace("axp2101")
AXP2101 = ns.class_("AXP2101", cg.PollingComponent, i2c.I2CDevice)


def validate_rails(config):
    if config["aldo_voltage"] % 100:
        raise cv.Invalid("ALDO voltage must use 100 mV steps")
    if config["reset_mask"] & ~config["aldo_mask"]:
        raise cv.Invalid("Reset rails must be included in aldo_mask")
    return config


CONFIG_SCHEMA = cv.All(cv.Schema({
    cv.GenerateID(): cv.declare_id(AXP2101),
    cv.Required("aldo_voltage"): cv.int_range(min=500, max=3500),
    cv.Required("aldo_mask"): cv.int_range(min=0, max=15),
    cv.Optional("reset_mask", default=0): cv.int_range(min=0, max=15),
    cv.Optional("battery_level"): sensor.sensor_schema(
        unit_of_measurement="%", accuracy_decimals=0, device_class="battery", state_class="measurement"),
    cv.Optional("battery_charging"): binary_sensor.binary_sensor_schema(device_class="battery_charging"),
}).extend(cv.polling_component_schema("60s")).extend(i2c.i2c_device_schema(0x34)), validate_rails)


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await i2c.register_i2c_device(var, config)
    cg.add(var.set_aldo_voltage(config["aldo_voltage"]))
    cg.add(var.set_aldo_mask(config["aldo_mask"]))
    cg.add(var.set_reset_mask(config["reset_mask"]))
    if "battery_level" in config:
        cg.add(var.set_battery_level(await sensor.new_sensor(config["battery_level"])))
    if "battery_charging" in config:
        cg.add(var.set_battery_charging(await binary_sensor.new_binary_sensor(config["battery_charging"])))
