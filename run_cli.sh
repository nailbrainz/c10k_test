rm -f err_cli_log.ini

for ((i=0;i<4;i++))
do
    echo "Running loop "$i
    ./c10k_cli.out 127.0.0.1 $i & #>>err_cli_log.ini 2>&1 &
    sleep 1
done

