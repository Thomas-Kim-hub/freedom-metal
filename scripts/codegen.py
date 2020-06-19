#!/usr/bin/env python3
# Copyright (c) 2020 SiFive Inc.
# SPDX-License-Identifier: Apache-2.0

import argparse
import sys

import jinja2
import pydevicetree

METAL_APIS = [
    "clock"
    "cpu",
    "interrupt",
    "shutdown",
    "uart",
]

DEFAULT_TEMPLATE_PATHS = [
    "../templates",
]

DEFAULT_CLOCK_DRIVERS = [
    "fixed-clock",
]

DEFAULT_INTERRUPT_DRIVERS = [
    "riscv,cpu-intc",
    "riscv,plic0",
]

def parse_arguments(argv):
    arg_parser = argparse.ArgumentParser(
            description="Generate Freedom Metal code from the target Devicetree")

    arg_parser.add_argument("-d", "--dts",
            required=True,
            help="The path to the target Devicetree")
    arg_parser.add_argument("-o", "--output-dir",
            required=True,
            help="The path to the directory to output generated code")

    arg_parser.add_argument("--template-paths",
            nargs='*',
            default=DEFAULT_TEMPLATE_PATHS,
            help="The paths to look for template")

    arg_parser.add_argument("--uart-driver", default="sifive,uart0",
            help="The driver for the UART API")
    arg_parser.add_argument("--shutdown-driver", default="sifive,test0",
            help="The driver for the Shutdown API")

    arg_parser.add_argument("--clock-drivers",
            nargs='*',
            default=DEFAULT_CLOCK_DRIVERS,
            help="The drivers for the clock API")
    arg_parser.add_argument("--interrupt-drivers",
            nargs='*',
            default=DEFAULT_INTERRUPT_DRIVERS,
            help="The drivers for the interrupt API")

    return arg_parser.parse_args(argv)

def to_snakecase(s):
    return s.lower().replace(',', '_').replace('-', '_')

driver_ids = dict()

def assign_ids(dts, args):
    drivers = []
    drivers.append(args.uart_driver)
    drivers.append(args.shutdown_driver)
    drivers += args.clock_drivers
    drivers += args.interrupt_drivers

    for driver in drivers:
        for node_id, node in enumerate(dts.match(driver)):
            driver_ids[node] = node_id

def node_to_dict(node, dts):
    d = dict()
    for prop in node.properties:
        key = to_snakecase(prop.name)
        values = []
        for value in prop.values:
            if isinstance(value, pydevicetree.ast.LabelReference):
                values.append(node_to_dict(dts.get_by_reference(value), dts))
            else:
                values.append(value)
        d[key] = values

    if node in driver_ids:
        d['id'] = driver_ids[node]

    return d

def main():
    args = parse_arguments(sys.argv[1:])

    dts = pydevicetree.Devicetree.parseFile(
            args.dts, followIncludes=True)

    # Assign driver IDs to all device instances
    assign_ids(dts, args)

    # Convert the Devicetree object tree into dictionary data
    # which can be rendered by the templates
    template_data = {
        'uarts' : [node_to_dict(uart, dts) for uart in dts.match(args.uart_driver)],
    }

    import pprint
    pprint.pprint(template_data)

if __name__ == "__main__":
    main()
