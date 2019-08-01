/* Copyright 2018 SiFive, Inc */
/* SPDX-License-Identifier: Apache-2.0 */

#include <metal/machine/platform.h>

#ifdef METAL_SIFIVE_LOCAL_EXTERNAL_INTERRUPTS0

#include <metal/io.h>
#include <metal/drivers/sifive_local-external-interrupts0.h>
#include <metal/machine.h>

void __metal_driver_sifive_local_external_interrupt_init(struct metal_interrupt *controller)
{
    struct __metal_driver_sifive_local_external_interrupts0 *local0;

    local0 = (struct __metal_driver_sifive_local_external_interrupts0 *)(controller);
    if ( !local0->init_done ) {
        struct metal_interrupt *intc =
	    __metal_driver_sifive_local_external_interrupts0_interrupt_parent(controller);

	if (intc) {
	    /* Register its interruptswith with parent controller, aka all external to default isr */
	    for (int i = 0;
		 i < __metal_driver_sifive_local_external_interrupts0_num_interrupts(controller);
		 i++) {
	        intc->vtable->interrupt_register(intc,
		    __metal_driver_sifive_local_external_interrupts0_interrupt_lines(controller, i),
					   NULL, controller);
	    }
	    local0->init_done = 1;
	}
    }
}

int __metal_driver_sifive_local_external_interrupt_register(struct metal_interrupt *controller,
                                                          int id, metal_interrupt_handler_t isr,
                                                          void *priv)
{
    int rc = -1;

    if (id != 0) {
        struct metal_interrupt *intc =
	    __metal_driver_sifive_local_external_interrupts0_interrupt_parent(controller);

        /* Enable its interrupts with parent controller */
        if (intc) {
            rc = intc->vtable->interrupt_register(intc, id, isr, priv);
        }
    }
    return rc;
}

int __metal_driver_sifive_local_external_interrupt_enable(struct metal_interrupt *controller, int id)
{
    int rc = -1;

    if (id != 0) {
        struct metal_interrupt *intc =
	    __metal_driver_sifive_local_external_interrupts0_interrupt_parent(controller);

        /* Enable its interrupts with parent controller */
        if (intc) {
            rc = intc->vtable->interrupt_enable(intc, id);
        }
    }
    return rc;
}

int __metal_driver_sifive_local_external_interrupt_disable(struct metal_interrupt *controller, int id)
{
    int rc = -1;

    if (id != 0) {
        struct metal_interrupt *intc =
	    __metal_driver_sifive_local_external_interrupts0_interrupt_parent(controller);

        /* Enable its interrupts with parent controller */
        if (intc) {
            rc = intc->vtable->interrupt_disable(intc, id);
        }
    }
    return rc;
}

int __metal_driver_sifive_local_external_command_request (struct metal_interrupt *controller,
                                                        int command, void *data)
{
    int idx;
    int rc = -1;

    switch (command) {
    case METAL_MAX_INTERRUPT_GET:
        rc = __metal_driver_sifive_local_external_interrupts0_num_interrupts(controller);
        break;
    case METAL_INDEX_INTERRUPT_GET:
        rc = 0;
        if (data) {
            idx = *(int *)data;
            rc = __metal_driver_sifive_local_external_interrupts0_interrupt_lines(controller, idx);
        }
        break;
    default:
        break;
    }

    return rc;
}

__METAL_DEFINE_VTABLE(__metal_driver_vtable_sifive_local_external_interrupts0) = {
    .local0_vtable.interrupt_init     = __metal_driver_sifive_local_external_interrupt_init,
    .local0_vtable.interrupt_register = __metal_driver_sifive_local_external_interrupt_register,
    .local0_vtable.interrupt_enable   = __metal_driver_sifive_local_external_interrupt_enable,
    .local0_vtable.interrupt_disable  = __metal_driver_sifive_local_external_interrupt_disable,
    .local0_vtable.command_request    = __metal_driver_sifive_local_external_command_request,
};

#endif

typedef int no_empty_translation_units;

