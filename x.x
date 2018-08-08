53c53
<       config/GreenADC.C config/GreenADC18.C 
---
>       config/GreenADC.C config/GreenADC18.C
66c66
< 	adc/HAPADC_rspec.o  adc/HAPADC_test.o  adc/HAPADC_config.o
---
> 	adc/HAPADC_rspec.o adc/HAPADC_test.o adc/HAPADC_config.o
76,78c76,77
< AUTO = auto/auto_rhwp.o auto/auto_filter.o auto/auto_led.o
< #FLEXIO = flexio/flexio_lib_ch.o flexio/flexio_lib_rspec.o
< FLEXIO = flexio/flexioLib.o
---
> AUTO = auto/auto.o auto/auto_filter.o
> FLEXIO = flexio/flexio_lib.o
120,122c119,120
< #	rm -f flexio/flexio_lib_ch.o flexio/flexio_lib_rspec.o
< 	rm -f flexio/flexioLib.o
< 	rm -r auto/auto_rhwp.o auto/auto_filter.o auto/auto_led.o
---
> 	rm -f flexio/flexio_lib.o
> 	rm -r auto/auto.o auto/auto_filter.o
131a130
> 
184c183
< flexio/flexioLib.o : flexio/flexioLib.c flexio/flexioLib.h
---
> flexio/flexio_lib.o : flexio/flexio_lib.c
186,194c185
< 	ccppc -o $@ $(CCVXFLAGS) flexio/flexioLib.c
< 
< # flexio/flexio_lib_ch.o : flexio/flexio_lib.c flexio/flexio.h
< # 	rm -f $@
< # 	ccppc -o $@ $(CCVXFLAGS) -DCOUNTINGHOUSE flexio/flexio_lib.c
< 
< # flexio/flexio_lib_rspec.o : flexio/flexio_lib.c flexio/flexio.h
< # 	rm -f $@
< # 	ccppc -o $@ -c $(CCVXFLAGS) -DRIGHTSPECT flexio/flexio_lib.c
---
> 	ccppc -c $@ -c $(CCVXFLAGS) flexio/flexio_lib.c
229,233c220
< auto/auto_rhwp.o: auto/auto_rhwp.c
< 	cd auto; rm -f $@; \
< 	ccppc -c $(CCVXFLAGS) auto_rhwp.c
< 
< auto/auto_led.o: auto/auto_led.c auto/auto_led.h
---
> auto/auto.o: auto/auto.c
235c222
< 	ccppc -c $(CCVXFLAGS) auto_led.c
---
> 	ccppc -c $(CCVXFLAGS) auto.c
