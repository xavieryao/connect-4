#File: setup.py
#!/usr/bin/env python3

from distutils.core import setup, Extension

#生成一个扩展模块
pht_module = Extension('_strategy', #模块名称，必须要有下划线
                        sources=['strategy_wrap.cxx', #封装后的接口cxx文件
                                 'Judge.cpp',		#以下为原始代码所依赖的文件
                                 'Strategy.cpp',
                                 'UCTStrategy.cpp'
                                ],
                      )

setup(name = 'strategy',	#打包后的名称
        version = '0.1',
        author = 'SWIG Docs',
        description = 'Simple swig pht from docs',
        ext_modules = [pht_module], #与上面的扩展模块名称一致
        py_modules = ['strategy'], #需要打包的模块列表
    )
