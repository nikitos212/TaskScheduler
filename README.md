# Шедулер задач

## Описание

Класс отвечающий за выполнение связанных по данным между собой задач.
Часто, чтобы решить какую либо задачу, требуется выполнить граф вычислений, где узел графа это задача, ребро связь между результатом выполнения одной задачи и параметром для запуска другой. Реализован класс **TTaskScheduler** , решающий подобную задачу.

Пример такой задачи - нахождение корней квадратного уравнения (предполагаем что коэффициенты гарантирует что корня 2). Решение подобной задачи в лоб - подразумевает вычисление корня из дискриминанта дважды.  Очевидно, что это не оптимально.


Вот так мог бы выглядеть код для решение данной задачи с помощью TTaskScheduler

```cpp
struct AddNumber {
  float add(float a) const {
    return a + number;
  }

  float number;
};

float a = 1;
float b = -2;
float c = 0;
AddNumber add{
  .number = 3
};

TTaskScheduler scheduler;

auto id1 = scheduler.add([](float a, float c) {return -4 * a * c;}, a, c);

auto id2 = scheduler.add([](float b, float v) {return b * b + v;}, b, scheduler.getFutureResult<float>(id1));

auto id3 = scheduler.add([](float b, float d) {return -b + std::sqrt(d);}, b, scheduler.getFutureResult<float>(id2));

auto id4 = scheduler.add([](float b, float d) {return -b - std::sqrt(d);}, b, scheduler.getFutureResult<float>(id2));

auto id5 = scheduler.add([](float a, float v) {return v/(2*a);}, a, scheduler.getFutureResult<float>(id3));

auto id6 = scheduler.add([]{float a, float v} {return v/(2*a);}, a, scheduler.getFutureResult<float>(id4));

auto id7 = scheduler.add(&AddNumber::add, add, scheduler.getFutureResult<float>(id6));

scheduler.executeAll();

std::cout << "x1 = " << scheduler.getResult<float>(id5) << std::endl;
std::cout << "x2 = " << scheduler.getResult<float>(id6) << std::endl;
std::cout << "x3 = " << scheduler.getResult<float>(id7) << std::endl;
```

Где getFutureResult это результат выполнения задачи в будущем,

### Публичный интефейс TTaskScheduler

 - **add** - принимает в качестве аргумента задание для него. Возвращает объект описывающий добавленную таску.
 - **getFutureResult<T>** - возвращает объект, из которого в будущем можно получить результат задания, переданного в качестве результата типа Т
 - **getResult<T>** - возвращает результат выполнения задания определенного типа. Вычисляет его если оно еще не подсчитано, при это не происходит вычисления не нужных заданий
 - **executeAll** - выполняет все запланированные задания

## Тесты

Весь вышеуказанный класс покрыт тестами, с помощью фреймворка [Google Test](http://google.github.io/googletest).
