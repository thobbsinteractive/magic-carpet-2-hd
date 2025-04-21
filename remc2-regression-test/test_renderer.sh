
REMC2_EXECUTABLE=./remc2

test_level() {
    level=$1
    echo "Testing renderer in level $level"
    ASAN_OPTIONS=detect_leaks=0 ${REMC2_EXECUTABLE} --test_renderers --set_level $level --config_file_path ./renderer-config.ini | grep "No differences between HD and Original renderer and all checkpoints hit"
    
    if [ ${PIPESTATUS[1]} -eq 0 ]; then
        echo "test level $level ok"
    else
        echo "test level $level failed"
    fi
}

for i in `seq 0 1`; do
    test_level $i
done
