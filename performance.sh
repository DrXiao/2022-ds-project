sh -c "echo 0 > /proc/sys/kernel/randomize_va_space"

for i in /sys/devices/system/cpu/cpu*/cpufreq/scaling_governor
do
    echo performance > ${i}
done

sh -c "echo 1 > /sys/devices/system/cpu/intel_pstate/no_turbo"

cat /proc/sys/kernel/randomize_va_space
cat /sys/devices/system/cpu/cpu*/cpufreq/scaling_governor
cat /sys/devices/system/cpu/intel_pstate/no_turbo
