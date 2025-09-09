-- 八皇后问题（Lua 4/e 官方实现整理）
-- 用法：lua 8queen.lua

local N = 8                          -- 棋盘大小

-- 检查 (n,c) 是否不会被攻击
-- a   : 数组，a[i]=j 表示第 i 行皇后放在第 j 列
-- n   : 当前准备放置的行号
-- c   : 当前准备放置的列号
local function isplaceok (a, n, c)
 for i = 1, n - 1 do              -- 之前的每一行
  if (a[i] == c)                or      -- 同列
          (a[i] - i == c - n)        or      -- 主对角线
          (a[i] + i == c + n)                -- 副对角线
  then
   return false               -- 会被攻击
  end
 end
 return true                        -- 位置有效
end

-- 打印棋盘
local function printsolution (a)
 for i = 1, N do                    -- 每一行
  for j = 1, N do                -- 每一列
   io.write(a[i] == j and "X" or "-", " ")
  end
  io.write("\n")
 end
 io.write("\n")
end

-- 递归放置皇后
-- a : 棋盘数组（1~N）
-- n : 当前要放的行（1~N+1）
local function addqueen (a, n)
 if n > N then                      -- 全部放完
  printsolution(a)
 else
  for c = 1, N do                -- 逐列尝试
   if isplaceok(a, n, c) then
    a[n] = c               -- 放置
    addqueen(a, n + 1)     -- 下一行
   end
  end
 end
end

-- 入口：空棋盘，从第 1 行开始
addqueen({}, 1)