# eigen-cont-malloc

A sandbox where I look at parallelizing computations in Eigen

Be sure to disable Turboboost:

```bash
echo "1" | sudo tee /sys/devices/system/cpu/intel_pstate/no_turbo
```

## results

### `-march=native`

![march=native](marchnative/time-vs-size-vs-threads.png)

### no `-march=native`

![no march=native](nomarchnative/time-vs-size-vs-threads.png)
