# Data Structures: Implementation and Experimental Analysis

Курсовой проект посвящён реализации, тестированию и экспериментальному анализу структур данных на Python и C++.

## Цели проекта

В рамках проекта рассматриваются следующие структуры данных:

- Hash Table;
- Binary Heap;
- KD-Tree;

Для каждой структуры предполагается:

- реализовать собственную версию на Python и C++;
- написать тесты корректности;
- подготовить benchmarks;
- сравнить с готовыми аналогами языка или библиотеки;
- сохранить результаты измерений в CSV;
- построить таблицы и графики;
- проанализировать поведение структуры на разных сценариях.

Проект носит исследовательский характер: важен не только сам код, но и сравнение асимптотики, практической скорости, влияния входных данных и особенностей реализации.

## Структура репозитория

```text
my-ds/
├── cpp/
│   ├── benchmarks/     # benchmark-сценарии для C++
│   ├── src/            # реализации структур данных на C++
│   ├── tests/          # тесты C++-реализаций
│   └── CMakeLists.txt
│
├── python/
│   ├── analysis/       # обработка CSV, построение таблиц и графиков
│   ├── benchmarks/     # benchmark-сценарии для Python
│   ├── src/            # реализации структур данных на Python
│   └── tests/          # тесты Python-реализаций
│           
├── results/
│   ├── plots/          # текстовые таблицы с результатами
│   ├── processed/      # обработанные CSV
│   ├── raw/            # сырые результаты benchmarks в CSV
│   └── tables/         # текстовые таблицы с результатами
│           
├── .gitignore
├── LICENSE
└── README.md            
```

## Команды запуска

Все команды ниже предполагают запуск **из корня проекта**.

---

### Python: запуск тестов

```bash
PYTHONPATH=python/src python -m pytest python/tests
```

Проверяет корректность Python-реализаций структур данных.

---

### Python: запуск benchmarks

```bash
PYTHONPATH=python/src python python/benchmarks/benchmark_hash_table.py > results/raw/python_hash_table_raw.csv
```

```bash
PYTHONPATH=python/src python python/benchmarks/benchmark_binary_heap.py > results/raw/python_binary_heap_raw.csv
```

```bash
PYTHONPATH=python/src python python/benchmarks/benchmark_kd_tree.py > results/raw/python_kd_tree_raw.csv
```

Запускают benchmark-сценарии для соответствующих структур данных.

Результаты сохраняются в CSV-файлы в директорию:

```text
results/raw/
```

---

### Python: обработка сырых результатов

Общий формат команды:

```bash
python python/analysis/summarize_results.py <input_raw_csv> <output_summary_csv>
```

Пример для хеш-таблицы:

```bash
python python/analysis/summarize_results.py results/raw/python_hash_table.csv results/processed/python_hash_table_summary.csv
```

Скрипт читает сырой CSV с отдельными запусками benchmark-ов и строит агрегированную таблицу с основными статистиками:

- среднее значение;
- медиана;
- стандартное отклонение;
- стандартная ошибка.

Итоговый CSV сохраняется в директорию:

```text
results/processed/
```

---

### Python: построение текстовых таблиц

Общий формат команды:

```bash
python python/analysis/print_tables.py <summary_csv> <output_txt> <first_implementation> <second_implementation>
```

Пример для бинарной кучи:

```bash
python python/analysis/print_tables.py results/processed/python_binary_heap_summary.csv results/tables/python_binary_heap_tables.txt BinaryHeap heapq
```

Скрипт строит текстовые таблицы сравнения двух реализаций по каждому benchmark-сценарию.

Например:

- собственная `HashTable` против Python `dict`;
- собственная `BinaryHeap` против Python `heapq`.

Итоговые таблицы сохраняются в директорию:

```text
results/tables/
```

---

### Python: построение графиков

Общий формат команды:

```bash
python python/analysis/plot_results.py <summary_csv> --plots-dir <plots_dir>
```

Пример для одного файла:

```bash
python python/analysis/plot_results.py results/processed/python_hash_table_summary.csv --plots-dir results/plots
```

Пример для нескольких файлов:

```bash
python python/analysis/plot_results.py results/processed/python_hash_table_summary.csv \
  results/processed/cpp_hash_table_summary.csv --plots-dir results/plots
```

Скрипт группирует результаты по структуре данных и benchmark-сценарию, после чего сохраняет графики в директорию:

```text
results/plots/
```

---

### C++: сборка проекта

```bash
cmake -S cpp -B cpp/build
cmake --build cpp/build
```

Первая команда настраивает CMake-проект.

Вторая команда компилирует C++-реализации, тесты и benchmarks.

---

### C++: запуск тестов

```bash
ctest --test-dir cpp/build
```

Запускает все C++-тесты, зарегистрированные через CMake.

Также можно запустить конкретный тест вручную:

```bash
./cpp/build/test_hash_table
```

```bash
./cpp/build/test_binary_heap
```

```bash
./cpp/build/test_kd_tree
```

---

### C++: запуск benchmarks

```bash
./cpp/build/benchmark_hash_table > results/raw/cpp_hash_table_raw.csv
```

```bash
./cpp/build/benchmark_binary_heap > results/raw/cpp_binary_heap_raw.csv
```

```bash
./cpp/build/benchmark_kd_tree > results/raw/cpp_kd_tree_raw.csv
```

Запускают benchmark-сценарии для C++-реализаций.

Результаты сохраняются в CSV-файлы в директорию:

```text
results/raw/
```