# Usage of utils scripts

## Parse CSV

### Compile from source

```
g++ -o parseCsv parse_csv.cpp
```

### Usage

```
./parseCsv -i "../measurements/measurement_509.csv" -o "parsed_data.csv" -c "0, 1, 4" -ci 9999
```

## Split data

### Compile from source

```
g++ -o splitData split_data.cpp
```

### Usage

```
./splitData -i "parsed_data.csv" -he 7200 -cs 13500 --heeting --cooling
```
