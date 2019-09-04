#ifndef NATIVE_CPP_
#define NATIVE_CPP_
#include "process.h"
#include <iostream>
#include "natives.cpp"

namespace PROCESS
{

///////////////////////////////////
// APPLY
template <typename F, typename T, std::size_t N, std::size_t... Idx>
decltype(auto) apply_impl(F f, T (&t)[N], std::index_sequence<Idx...>)
{
  return f(t[Idx]...);
}

template <typename F, typename T, std::size_t N>
decltype(auto) apply(F f, T (&t)[N])
{
  return apply_impl(f, t, std::make_index_sequence<N>{});
}

// APPLY ->>

template <typename _Fn, typename _Tuple, std::size_t... _Idx>
constexpr decltype(auto)
__apply_impl(_Fn &&f, _Tuple &&t, std::index_sequence<_Idx...>)
{
  return std::forward<_Fn>(f)(std::get<_Idx>(std::forward<_Tuple>(t))...);
}

template <typename _Fn, typename _Tuple>
constexpr decltype(auto)
apply2(_Fn &&f, _Tuple &&t)
{
  using _Indices =
      std::make_index_sequence<std::tuple_size<std::decay_t<_Tuple>>::value>;
  return __apply_impl(std::forward<_Fn>(f), std::forward<_Tuple>(t),
                      _Indices{});
}
//->>

template <typename Func, Func f>
struct store_func_helper;

std::string demangle(const std::string &val)
{
  int status;
  char *realname;

  std::string strname = realname = abi::__cxa_demangle(val.c_str(), 0, 0, &status);
  free(realname);
  return strname;
}

// args will be implicitly converted to arg<T>::type before calling function
// default: convert to const Arg
template <typename Arg, typename snifae = void>
struct arg
{
  using type = const Arg &;
};

// numeric types: convert to double.
template <typename Arg>
struct arg<Arg, typename std::enable_if<std::is_arithmetic<Arg>::value, void>::type>
{
  using type = double;
};

// set more special arg types here.

// Functions stored in the map are first wrapped in a lambda with this signature.
template <typename Ret, typename... Arg>
using func_type = Ret (*)(typename arg<Arg>::type...);

/*template <typename Ret, typename... Args>
Ret call2(int func, Args... args)
{
  using new_func_type = func_type<Ret, Args...>;
  auto &mapVal = n->m_func_map.at(func);

  if (mapVal.second != std::type_index(typeid(new_func_type)))
  {
    std::ostringstream ss;
    ss << "Error calling function " << func << ", function type: "
       << demangle(mapVal.second.name())
       << ", attempted to call with " << demangle(typeid(new_func_type).name());
    throw std::runtime_error(ss.str());
  }

  auto typeCastedFun = (new_func_type)(mapVal.first.func);

  //args will be implicitly converted to match standardized args
  //return apply(typeCastedFun,p);
  return typeCastedFun(std::forward<Args>(args)...);
};*/

Native *Native::instance = 0;

Native *Native::getInstance()
{
  if (instance == 0)
  {
    instance = new Native();
  }

  return instance;
}

Native::Native()
{
  void *handle;
  handle = dlopen("./libstdn.so", RTLD_NOW);

    if (!handle)
    {
      cout<<dlerror()<<endl;
    }else{
      cout<<"loaded"<<endl;
    }
#if DEBUG
  cout << "new native" << endl;
#endif
  
  MetaFunction metaPrintLn;
  metaPrintLn.argc = 1;
  metaPrintLn.signature = "(string)I";
  metaPrintLn.type = 0;
  metaPrintLn.name = "println";
  FUNC_MAP_STORE(*this, println, metaPrintLn);

  MetaFunction metaPrnt;
  metaPrnt.argc = 1;
  metaPrnt.signature = "(string)I";
  metaPrnt.type = 0;
  metaPrnt.name = "print";
  FUNC_MAP_STORE(*this, print, metaPrnt);

  MetaFunction metaSum;
  metaSum.argc = 2;
  metaSum.signature = "(II)I";
  metaSum.type = 1;
  metaSum.name = "sum";
  //metaSum.func = (void (*)())sum;
  FUNC_MAP_STORE(*this, sum, metaSum);

  MetaFunction meta_sleep;
  meta_sleep.argc = 1;
  meta_sleep.signature = "(I)I";
  meta_sleep.type = 1;
  meta_sleep.name = "native_sleep";
  FUNC_MAP_STORE(*this, native_sleep, meta_sleep);

  MetaFunction meta_u_sleep;
  meta_u_sleep.argc = 1;
  meta_u_sleep.signature = "(I)I";
  meta_u_sleep.type = 1;
  meta_u_sleep.name = "native_u_sleep";
  FUNC_MAP_STORE(*this, native_u_sleep, meta_u_sleep);
 
  MetaFunction metaItos;
  metaItos.argc = 1;
  metaItos.signature = "(I)string";
  metaItos.type = 1;
  metaItos.name = "native_itos";
  FUNC_MAP_STORE(*this, native_itos, metaItos);

  //n = this;
}

template <typename Func, Func f>
void Native::store(const std::string &name, MetaFunction meta)
{
  store_func_helper<Func, f>::call(this, name, meta);
}

template <typename Ret, typename... Args>
Ret Native::call(int func, Args... args)
{
  using new_func_type = func_type<Ret, Args...>;
  auto &mapVal = m_func_map.at(func);

  if (mapVal.second != std::type_index(typeid(new_func_type)))
  {
    std::ostringstream ss;
    ss << "Error calling function " << func << ", function type: "
       << demangle(mapVal.second.name())
       << ", attempted to call with " << demangle(typeid(new_func_type).name());
    throw std::runtime_error(ss.str());
  }

  auto typeCastedFun = (new_func_type)(mapVal.first.func);

  //args will be implicitly converted to match standardized args
  //return apply(typeCastedFun,p);
  return typeCastedFun(std::forward<Args>(args)...);
};

template <typename Ret>
Ret Native::call_wrap(int funcid /* recibe cpu */) //test borrar
{
  auto &mapVal = m_func_map.at(funcid);
  MetaFunction meta = mapVal.first;
  std::cout << "id: " << meta.id << std::endl;
  std::cout << "argc: " << meta.argc << std::endl;
  std::cout << "name: " << meta.name << std::endl;
  std::cout << "signarure: " << meta.signature << std::endl;

  const int c = (meta.argc);
  std::cout << "argc: " << c << std::endl;

  switch (c)
  case 2:
  {
    std::array<int, 3> nc;
    ;
    //int nc[3];
    nc[0] = funcid;
    nc[1] = 1;
    nc[2] = 2;

    return apply2(
        //std::forward<decltype(xs)>
        [this](auto... xs) { return this->call<Ret>(xs...); },
        nc);
    break;
  }

  //return this->call<Ret>(funcid,arg);
}

template <typename Ret>
Ret Native::call_wrap2(int funcid, PROCESS::Cpu *cpu)
{
  this->heap = cpu->process->heap;
#if DEBUG
  cout << " call_wrap2 funcid: " << funcid << endl;
#endif
  auto &mapVal = m_func_map.at(funcid);
  MetaFunction meta = mapVal.first;
#if DEBUG
  std::cout << "argc: " << meta.argc << std::endl;
  std::cout << "name: " << meta.name << std::endl;
  std::cout << "signarure: " << meta.signature << std::endl;
#endif

  const unsigned int c = (meta.argc);

  { //CODE GENERATED WITH 20 CASES.
    switch (c)
    {
      //case 0; generated.
    case 0:
      std::array<int, 1> _params__0;

      _params__0[0] = funcid;

      return apply2(
          //std::forward<decltype(xs)>
          [this](auto... xs) { return this->call<Ret>(xs...); },
          _params__0);
      break;

      //CASE 1; GENERATED.
    case 1:
      std::array<int, 2> _params_1;

      _params_1[0] = funcid;

      for (int x = 1; x >= 1; x--)
      {
        _params_1[x] = cpu->estack->Pop();
#if DEBUG
        cout << "x: " << x << ", VAL: " << _params_1[x] << endl;
#endif
      }

      return apply2(
          //std::forward<decltype(xs)>
          [this](auto... xs) { return this->call<Ret>(xs...); },
          _params_1);
      break;

      //CASE 2; GENERATED.
    case 2:
      std::array<int, 3> _params_2;

      _params_2[0] = funcid;

      for (int x = 2; x >= 1; x--)
      {
        _params_2[x] = cpu->estack->Pop();
#if DEBUG
        cout << "x: " << x << ", VAL: " << _params_2[x] << endl;
#endif
      }

      return apply2(
          //std::forward<decltype(xs)>
          [this](auto... xs) { return this->call<Ret>(xs...); },
          _params_2);
      break;

      //CASE 3; GENERATED.
    case 3:
      std::array<int, 4> _params_3;

      _params_3[0] = funcid;

      for (int x = 3; x >= 1; x--)
      {
        _params_3[x] = cpu->estack->Pop();
#if DEBUG
        cout << "x: " << x << ", VAL: " << _params_3[x] << endl;
#endif
      }

      return apply2(
          //std::forward<decltype(xs)>
          [this](auto... xs) { return this->call<Ret>(xs...); },
          _params_3);
      break;

      //CASE 4; GENERATED.
    case 4:
      std::array<int, 5> _params_4;

      _params_4[0] = funcid;

      for (int x = 4; x >= 1; x--)
      {
        _params_4[x] = cpu->estack->Pop();
#if DEBUG
        cout << "x: " << x << ", VAL: " << _params_4[x] << endl;
#endif
      }

      return apply2(
          //std::forward<decltype(xs)>
          [this](auto... xs) { return this->call<Ret>(xs...); },
          _params_4);
      break;

      //CASE 5; GENERATED.
    case 5:
      std::array<int, 6> _params_5;

      _params_5[0] = funcid;

      for (int x = 5; x >= 1; x--)
      {
        _params_5[x] = cpu->estack->Pop();
#if DEBUG
        cout << "x: " << x << ", VAL: " << _params_5[x] << endl;
#endif
      }

      return apply2(
          //std::forward<decltype(xs)>
          [this](auto... xs) { return this->call<Ret>(xs...); },
          _params_5);
      break;

      //CASE 6; GENERATED.
    case 6:
      std::array<int, 7> _params_6;

      _params_6[0] = funcid;

      for (int x = 6; x >= 1; x--)
      {
        _params_6[x] = cpu->estack->Pop();
#if DEBUG
        cout << "x: " << x << ", VAL: " << _params_6[x] << endl;
#endif
      }

      return apply2(
          //std::forward<decltype(xs)>
          [this](auto... xs) { return this->call<Ret>(xs...); },
          _params_6);
      break;

      //CASE 7; GENERATED.
    case 7:
      std::array<int, 8> _params_7;

      _params_7[0] = funcid;

      for (int x = 7; x >= 1; x--)
      {
        _params_7[x] = cpu->estack->Pop();
#if DEBUG
        cout << "x: " << x << ", VAL: " << _params_7[x] << endl;
#endif
      }

      return apply2(
          //std::forward<decltype(xs)>
          [this](auto... xs) { return this->call<Ret>(xs...); },
          _params_7);
      break;

      //CASE 8; GENERATED.
    case 8:
      std::array<int, 9> _params_8;

      _params_8[0] = funcid;

      for (int x = 8; x >= 1; x--)
      {
        _params_8[x] = cpu->estack->Pop();
#if DEBUG
        cout << "x: " << x << ", VAL: " << _params_8[x] << endl;
#endif
      }

      return apply2(
          //std::forward<decltype(xs)>
          [this](auto... xs) { return this->call<Ret>(xs...); },
          _params_8);
      break;

      //CASE 9; GENERATED.
    case 9:
      std::array<int, 10> _params_9;

      _params_9[0] = funcid;

      for (int x = 9; x >= 1; x--)
      {
        _params_9[x] = cpu->estack->Pop();
#if DEBUG
        cout << "x: " << x << ", VAL: " << _params_9[x] << endl;
#endif
      }

      return apply2(
          //std::forward<decltype(xs)>
          [this](auto... xs) { return this->call<Ret>(xs...); },
          _params_9);
      break;

      //CASE 10; GENERATED.
    case 10:
      std::array<int, 11> _params_10;

      _params_10[0] = funcid;

      for (int x = 10; x >= 1; x--)
      {
        _params_10[x] = cpu->estack->Pop();
#if DEBUG
        cout << "x: " << x << ", VAL: " << _params_10[x] << endl;
#endif
      }

      return apply2(
          //std::forward<decltype(xs)>
          [this](auto... xs) { return this->call<Ret>(xs...); },
          _params_10);
      break;

      //CASE 11; GENERATED.
    case 11:
      std::array<int, 12> _params_11;

      _params_11[0] = funcid;

      for (int x = 11; x >= 1; x--)
      {
        _params_11[x] = cpu->estack->Pop();
#if DEBUG
        cout << "x: " << x << ", VAL: " << _params_11[x] << endl;
#endif
      }

      return apply2(
          //std::forward<decltype(xs)>
          [this](auto... xs) { return this->call<Ret>(xs...); },
          _params_11);
      break;

      //CASE 12; GENERATED.
    case 12:
      std::array<int, 13> _params_12;

      _params_12[0] = funcid;

      for (int x = 12; x >= 1; x--)
      {
        _params_12[x] = cpu->estack->Pop();
#if DEBUG
        cout << "x: " << x << ", VAL: " << _params_12[x] << endl;
#endif
      }

      return apply2(
          //std::forward<decltype(xs)>
          [this](auto... xs) { return this->call<Ret>(xs...); },
          _params_12);
      break;

      //CASE 13; GENERATED.
    case 13:
      std::array<int, 14> _params_13;

      _params_13[0] = funcid;

      for (int x = 13; x >= 1; x--)
      {
        _params_13[x] = cpu->estack->Pop();
#if DEBUG
        cout << "x: " << x << ", VAL: " << _params_13[x] << endl;
#endif
      }

      return apply2(
          //std::forward<decltype(xs)>
          [this](auto... xs) { return this->call<Ret>(xs...); },
          _params_13);
      break;

      //CASE 14; GENERATED.
    case 14:
      std::array<int, 15> _params_14;

      _params_14[0] = funcid;

      for (int x = 14; x >= 1; x--)
      {
        _params_14[x] = cpu->estack->Pop();
#if DEBUG
        cout << "x: " << x << ", VAL: " << _params_14[x] << endl;
#endif
      }

      return apply2(
          //std::forward<decltype(xs)>
          [this](auto... xs) { return this->call<Ret>(xs...); },
          _params_14);
      break;

      //CASE 15; GENERATED.
    case 15:
      std::array<int, 16> _params_15;

      _params_15[0] = funcid;

      for (int x = 15; x >= 1; x--)
      {
        _params_15[x] = cpu->estack->Pop();
#if DEBUG
        cout << "x: " << x << ", VAL: " << _params_15[x] << endl;
#endif
      }

      return apply2(
          //std::forward<decltype(xs)>
          [this](auto... xs) { return this->call<Ret>(xs...); },
          _params_15);
      break;

      //CASE 16; GENERATED.
    case 16:
      std::array<int, 17> _params_16;

      _params_16[0] = funcid;

      for (int x = 16; x >= 1; x--)
      {
        _params_16[x] = cpu->estack->Pop();
#if DEBUG
        cout << "x: " << x << ", VAL: " << _params_16[x] << endl;
#endif
      }

      return apply2(
          //std::forward<decltype(xs)>
          [this](auto... xs) { return this->call<Ret>(xs...); },
          _params_16);
      break;

      //CASE 17; GENERATED.
    case 17:
      std::array<int, 18> _params_17;

      _params_17[0] = funcid;

      for (int x = 17; x >= 1; x--)
      {
        _params_17[x] = cpu->estack->Pop();
#if DEBUG
        cout << "x: " << x << ", VAL: " << _params_17[x] << endl;
#endif
      }

      return apply2(
          //std::forward<decltype(xs)>
          [this](auto... xs) { return this->call<Ret>(xs...); },
          _params_17);
      break;

      //CASE 18; GENERATED.
    case 18:
      std::array<int, 19> _params_18;

      _params_18[0] = funcid;

      for (int x = 18; x >= 1; x--)
      {
        _params_18[x] = cpu->estack->Pop();
#if DEBUG
        cout << "x: " << x << ", VAL: " << _params_18[x] << endl;
#endif
      }

      return apply2(
          //std::forward<decltype(xs)>
          [this](auto... xs) { return this->call<Ret>(xs...); },
          _params_18);
      break;

      //CASE 19; GENERATED.
    case 19:
      std::array<int, 20> _params_19;

      _params_19[0] = funcid;

      for (int x = 19; x >= 1; x--)
      {
        _params_19[x] = cpu->estack->Pop();
#if DEBUG
        cout << "x: " << x << ", VAL: " << _params_19[x] << endl;
#endif
      }

      return apply2(
          //std::forward<decltype(xs)>
          [this](auto... xs) { return this->call<Ret>(xs...); },
          _params_19);
      break;

      //CASE 20; GENERATED.
    case 20:
      std::array<int, 21> _params_20;

      _params_20[0] = funcid;

      for (int x = 20; x >= 1; x--)
      {
        _params_20[x] = cpu->estack->Pop();
#if DEBUG
        cout << "x: " << x << ", VAL: " << _params_20[x] << endl;
#endif
      }

      return apply2(
          //std::forward<decltype(xs)>
          [this](auto... xs) { return this->call<Ret>(xs...); },
          _params_20);
      break;
    }
  }
}

int Native::get(std::string name)
{
  bool present = false;

  // Key is not present
  if (names.find(name) == names.end())
    return -1;

  return names.at(name);
}

template <typename Ret, typename... Args, Ret (*f)(Args...)> 
struct store_func_helper<Ret (*)(Args...), f>
{
  static void call(Native *map, const std::string &name, MetaFunction meta)
  {
    using new_func_type = func_type<Ret, Args...>;
    // add a wrapper function, which takes standardized args.
    new_func_type lambda = [](typename arg<Args>::type... args) -> Ret {
      return (*f)(args...);
    };

    (&meta)->func = (void (*)())lambda;
    (&meta)->id = map->index;

    map->m_func_map.insert(std::make_pair(
        map->index,
        std::make_pair(meta, std::type_index(typeid(lambda)))));

    map->names.insert(std::make_pair(name, map->index));
#if DEBUG
    cout << "Register native id: " << map->index << ", name:" << name << ", sign: " << meta.signature << endl;
#endif
    //map->metas.insert(std::make_pair(map->index, meta));

    map->index++;
  }
};

} // namespace PROCESS

#endif