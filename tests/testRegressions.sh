echo "Make sure that you have compiled the game with TEST_REGRESSIONS_GAME in sub_main.cpp"
echo "Get the data files from https://github.com/turican0/remc2/tree/data"
echo "Extract the ZIP archives to \$data_location"
echo "Adjust the path to the remc2 executable"

remc2_executable=./build/Debug/inst/bin/remc2
data_location=~/.local/share/remc2/regression_tests

# !NOTE!: the regression files have to be exactly at ../memimages/regressions relative to the remc2 executable
memimages_path=$(realpath ./build/Debug/inst/memimages/)
regression_files=$memimages_path/regressions/

test_level() {
    level=$1
    echo "testing level $level"
    mkdir -p "${regression_files}"
    rm -f "${regression_files}/*"
    cp "${data_location}/level${level}/"* "${regression_files}"
    gamelevel="$(($level-1))"
    pushd ./build/Debug/inst/bin > /dev/null
    ./remc2 reglevel $gamelevel --mode_test_regressions_game --memimages_path $memimages_path 2>&1 | grep 'Regression compare sequence error'
    if [ ${PIPESTATUS[0]} -eq 20 ]; then
        echo "test level$level ok"
    else
        echo "test level$level failed"
    fi
    popd > /dev/null
}

for i in `seq 1 25`; do
    test_level $i
done
