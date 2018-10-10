
static void print(const char* str, int row);

// 入口必须写在最前面，叫什么名字倒无所谓
void start()
{
    // 1. 打招呼
    // 2. 测试是否能够调用函数
    // 3. 测试增加.data
    print("Hello C language!", 3);
    while(1);
}

static void print(const char* str, int row)
{
    #define LINE ((char*)0x0b8000)
    // C99 支持把变量写在任意地方，就像 C++
    for(int i=0; str[i]; i++)
    {
        LINE[row * 80 * 2 + i * 2] = str[i];
        LINE[row * 80 * 2 + i * 2 + 1] = 0x0f;
    }
}
