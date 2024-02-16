# Usage of utils scripts

## Parse CSV

### Compile from source

```
g++ -o parseCsv parse_csv.cpp
```

### Usage

Command-line arguments are:
* The input file (-i).
* The output file (-o).
* Columns in order which to include into output file (-c).
* Start index (CSV row) from which new output file CSV should be generated (-si).
* Cutoff index (CSV row) which will be last row of new output CSV file (-ci).

### Example of usage

```
./parseCsv -i "../measurements/509/data.csv" -o "parsed_data.csv" -c "0, 1, 4" -ci 9999
```

Or

```
./parseCsv -i "../measurements/511/data.csv" -o "parsed_data.csv" -c "0, 1, 2"
```

## Split data

### Compile from source

```
g++ -o splitData split_data.cpp
```

### Usage

Command-line arguments are:
* The input file (-i).
* The heating end time (s) (-he).
* The cooling start time (s) (-cs).
* Flags indicating the presence of heating (--heating) and cooling data (--cooling).

### Example of usage

```
./splitData -i "parsed_data.csv" -he 7200 -cs 13500 --heating --cooling
```
