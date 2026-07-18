bits 32
global isr_stub_table

extern exception_handler

%macro ISR_NOERR 1

global isr%1

isr%1:
    cli 
    call exception_handler

%endmacro

ISR_NOERR 0
ISR_NOERR 1
ISR_NOERR 2
ISR_NOERR 3
ISR_NOERR 4
ISR_NOERR 5
ISR_NOERR 6

isr_stub_table:
    dd isr0
