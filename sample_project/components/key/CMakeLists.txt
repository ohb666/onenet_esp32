idf_component_register(
    # 1. 列出所有 .c 源文件，需要包含 src/ 路径
    SRCS 
        "src/key.c"

    # 2. 指定 .h 头文件所在的目录
    INCLUDE_DIRS "inc"

    REQUIRES driver
)