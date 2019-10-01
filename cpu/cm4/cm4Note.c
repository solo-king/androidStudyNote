基本概念:
    NWEINT_GPM00_CON
        External Interrupt NWEINT_GPM00 Configuration Register
    NWEINT_GPM00_FLTCON0
        External Interrupt NWEINT_GPM00 Filter Configuration Register 0
    NWEINT_GPM00_MASK
        External Interrupt NWEINT_GPM00 MASK Register
        Enables the interruptInterrupt Service Routine
            0x0 = Enables interrupt, 
            0x1 = Masks interrupt
    NWEINT_GPM00_PEND
        External Interrupt NWEINT_GPM00 PEND Register
    TZPC_CMGP_0
        Secure mode configuration register (Secure register)
        Sets the secure mode,
            0x0 = Secure access only,
            0x1 = All transaction can access, 
            0x2 ~ 0x3 = Nonsecure access only
    SPI
        The interrupt that facilitates the distributor to route to any of the specified combination of processors
    SGI
        Software Generated Interrupts
    PPI
        The interrupt that is specific to a single processor
    NVIC(Nested Vectored Interrupt Controller)
        1. interrupt mode
            level:
                中断需要相应外设来取消，因此外设可以控制处理器重复执行相应的ISR
            pluse:
                由处理器时钟的上升沿同步采集，因此需要外设将中断保持至少一个时钟周期
    IRQ
    FRQ
    ISR
        Interrupt Service Routine
question:
    1. CmgpIrqType来源   
        typedef enum{
        ...
            CMGP_GPIOM0_0_IRQ = 20,
        ...
        }CmgpIrqType;

        cmgpEnableInterrupt(CMGP_GPIOM0_7_IRQ)//CMGP_GPIOM0_7_IRQ=27
             cmgpDrvEnableInterrupt( intNum )
                /**
                 *   
                 *   值位对应的 1<27 中断位
                 */
                __raw_writel( regValue , REG_SYSREG_INTC0_IEN_SET )
    2. NVIC_EnableIRQ(SYSREG0_CMGP_IRQn)实现
        chub拥有的中断数由UM中的Interrupt Source Mapping to CM4F Interrupt Handler决定
        //SYSREG0_CMGP_IRQn           = 17,
        NVIC_EnableIRQ(SYSREG0_CMGP_IRQn)
        NVIC->ICPR[((uint32_t)(IRQn) >> 5)] = (1 << ((uint32_t)(IRQn) & 0x1F));
        NVIC->ICPR[000 1 0001 >> 5 ] = 1<<17 = bit[17]    
    3. GIC SPI SGI NVIC之间的关系
