<#
.SYNOPSIS
    交互式 Git 仓库管理脚本 V4 (去特殊符号版)
    目标仓库: E:\Git_Repository\Watch_project\EmbeddedProject_Folder
    远程仓库: Gitee (zhang___qi/watch_project)
#>

# ==========================================
# 0. 环境初始化
# ==========================================
[Console]::OutputEncoding = [System.Text.Encoding]::UTF8
$OutputEncoding = [System.Text.Encoding]::UTF8
git config --global core.quotepath false
$REPO_PATH = "E:\Git_Repository\Watch_project\EmbeddedProject_Folder"
$GITEE_REMOTE = "https://gitee.com/zhang___qi/watch_project.git" 
$COLOR_GREEN = "Green"; $COLOR_RED = "Red"; $COLOR_WHITE = "White"; $COLOR_CYAN = "Cyan"; $COLOR_YELLOW = "Yellow"

# ==========================================
# 工具函数
# ==========================================
function Write-Info ($text, $color="White", $newline=$true) {
    if ($newline) { Write-Host $text -ForegroundColor $color }
    else { Write-Host $text -ForegroundColor $color -NoNewline }
}

function Show-Header {
    Clear-Host
    Write-Info "================================================================" $COLOR_CYAN
    Write-Info "    Git 交互式管理脚本 - Windows 终端 1.23 适配版 (V4)" $COLOR_GREEN
    Write-Info "    仓库: $REPO_PATH" $COLOR_WHITE
    Write-Info "================================================================" $COLOR_CYAN
}

function Wait-Input {
    Write-Info "`n按任意键返回主菜单..." $COLOR_WHITE
    $null = $Host.UI.RawUI.ReadKey("NoEcho,IncludeKeyDown")
}

function Get-CleanInput ($promptMsg) {
    # 自动去除首尾空格
    $val = Read-Host -Prompt $promptMsg
    if ($null -eq $val) { return "" }
    return $val.Trim()
}

# 路径校验
if (-not (Test-Path -Path $REPO_PATH)) {
    Write-Info "[错误] 找不到目标仓库路径: $REPO_PATH" $COLOR_RED; Pause; Exit
}
Set-Location -Path $REPO_PATH

# ==========================================
# 核心逻辑
# ==========================================

# --- 清理暂存区 ---
function Module-CleanStaging {
    Write-Info "`n正在执行清理 (git rm --cached & reset)..." $COLOR_YELLOW
    git rm -r --cached . 2>$null | Out-Null
    git reset HEAD . 2>$null | Out-Null
    Write-Info "暂存区已清理完毕。" $COLOR_GREEN
}

# --- 安全退出 ---
function Safe-Exit {
    Module-CleanStaging
    Write-Info "脚本已退出。" $COLOR_GREEN
    Exit
}

# --- 功能 1: 分支管理 ---
function Module-ManageBranches {
    Show-Header
    Write-Info "[功能1] 分支查看与本地分支删除" $COLOR_YELLOW
    Write-Info "正在同步远程分支信息..." $COLOR_WHITE
    git fetch $GITEE_REMOTE 2>&1 | Out-Null
    
    Write-Info "`n-------- 分支列表 --------" $COLOR_CYAN
    $branches = git branch -a
    $branchList = @()
    $i = 1

    foreach ($line in $branches) {
        $line = $line.Trim()
        $isCurrent = $line.StartsWith("*")
        $isRemote = $line.StartsWith("remotes/")
        $displayName = if ($isCurrent) { $line.Substring(2) } else { $line }
        
        $shortName = if ($displayName.Length -gt 25) { $displayName.Substring(0, 22) + "..." } else { $displayName }
        Write-Info -text "[$i] " -color $COLOR_WHITE -newline $false
        
        if ($isCurrent) { Write-Info -text "* $shortName (当前)" -color $COLOR_GREEN }
        elseif ($isRemote) { Write-Info -text "$shortName (远程)" -color $COLOR_RED }
        else { Write-Info -text "$shortName (本地)" -color $COLOR_WHITE }
        
        if (-not $isRemote -and -not $line.StartsWith("remotes")) {
            $cleanName = $displayName.Replace("* ", "").Trim()
            $branchList += @{Index=$i; Name=$cleanName}
        }
        $i++
    }

    $userInput = Get-CleanInput "`n请输入要删除的分支 名/序号 (回车返回)"
    
    if ($userInput -eq "Q") { Safe-Exit } # 改为 Q
    if ([string]::IsNullOrWhiteSpace($userInput)) { return }

    $targetBranch = ""
    if ($userInput -match "^\d+$") {
        $found = $branchList | Where-Object { $_.Index -eq [int]$userInput }
        if ($found) { $targetBranch = $found.Name }
    } else { $targetBranch = $userInput }

    if ([string]::IsNullOrWhiteSpace($targetBranch)) {
        Write-Info "分支无效。" $COLOR_RED; Wait-Input; return
    }
    
    if ($targetBranch -match "^(master|main)$") {
        Write-Info "无法直接删除保护分支 '$targetBranch'。" $COLOR_RED; Wait-Input; return
    }

    git branch -D $targetBranch
    Write-Info "分支已删除。" $COLOR_GREEN; Wait-Input
}

# --- 功能 2: 核心管理 (按键逻辑已更新) ---
function Module-FileAndBranch {
    # 阶段 1: 选分支
    while ($true) {
        Show-Header
        Write-Info "[功能2 - 阶段1] 分支切换" $COLOR_YELLOW
        $currentBranch = (git branch --show-current).Trim()
        Write-Info "当前分支: $currentBranch" $COLOR_GREEN
        
        $targetInput = Get-CleanInput "`n请输入目标分支名 (回车跳过, Q退出)"
        
        # 改为 Q 退出
        if ($targetInput -eq "Q") { Safe-Exit }
        
        if (-not [string]::IsNullOrWhiteSpace($targetInput)) {
            $check = git branch --list $targetInput
            if ($check) { 
                git checkout $targetInput 
            } else {
                $create = Get-CleanInput "分支不存在，是否创建? (Y/N)"
                if ($create -eq "Y") { git checkout -b $targetInput }
            }
        }

        # 阶段 2: 文件管理 (内层循环)
        $localRoot = $REPO_PATH
        $currentBrowsePath = $localRoot
        $selectedFiles = @()

        while ($true) {
            Show-Header
            Write-Info "[功能2 - 阶段2] 仓库文件管理" $COLOR_YELLOW
            Write-Info "当前路径: $currentBrowsePath" $COLOR_WHITE
            
            $headerFmt = "{0,-5} {1,-8} {2,-16} {3,-10} {4}"
            Write-Info ($headerFmt -f "索引", "Mode", "LastWriteTime", "Length", "Name") $COLOR_CYAN
            
            $items = Get-ChildItem -Path $currentBrowsePath | Sort-Object -Property @{Expression={$_.PSIsContainer}; Descending=$true}, Name
            $indexMap = @{}
            $idx = 1
            
            if ($currentBrowsePath -ne $localRoot) {
                Write-Info ($headerFmt -f "0", "d-----", "-", "-", ".. (返回上一级目录)") $COLOR_WHITE
            }

            foreach ($item in $items) {
                $len = if ($item.PSIsContainer) { "" } else { [math]::Round($item.Length/1KB, 1).ToString() + "KB" }
                $name = $item.Name
                if ($name.Length -gt 35) { $name = $name.Substring(0, 32) + "..." }
                
                $lineColor = $COLOR_WHITE
                $prefix = ""
                if ($selectedFiles -contains $item.FullName) {
                    $lineColor = $COLOR_GREEN; $prefix = "[已选] "
                } elseif ($item.PSIsContainer) {
                    $lineColor = $COLOR_YELLOW
                }

                Write-Info ($headerFmt -f $idx, $item.Mode, $item.LastWriteTime.ToString("yyyy/MM/dd HH:mm"), $len, ($prefix + $name)) $lineColor
                $indexMap[$idx] = $item
                $idx++
            }

            if ($selectedFiles.Count -gt 0) {
                Write-Info "`n[待提交池]:" $COLOR_GREEN
                $selectedFiles | Select-Object -Last 3 | ForEach-Object { Write-Info " + $_" $COLOR_GREEN }
                if ($selectedFiles.Count -gt 3) { Write-Info " ... (共 $($selectedFiles.Count) 个)" $COLOR_GREEN }
            }

            Write-Info "`n[操作指令]: 数字(选中/取消) | cd 数字(进入层级) | 0(上一级) | done(提交)" $COLOR_CYAN
            # 更新提示：Q=退出脚本, B=返回上一阶段
            Write-Info "[流程控制]: B(返回分支选择) | Q(安全退出脚本)" $COLOR_CYAN
            
            $cmd = Get-CleanInput "请输入"
            
            # 1. 安全退出 (Q)
            if ($cmd -eq "Q") { Safe-Exit }

            # 2. 返回阶段1 (B) - 原先是 q，现在改为 b 防止冲突
            if ($cmd -eq "B") { break } 

            # 3. 提交逻辑
            # 3. 提交逻辑（修复删除文件+未追踪文件问题）
            if ($cmd -eq "done") {
                # 步骤1：添加手动选中的文件
                if ($selectedFiles.Count -gt 0) {
                    foreach ($f in $selectedFiles) { git add "$f" }
                }
                # 步骤2：自动标记已追踪文件的删除/修改（解决deleted文件问题）
                git add -u 2>&1 | Out-Null
                # 步骤3：可选：自动添加所有未追踪的文件（解决Untracked files问题）
                # 如果你想手动选则注释这行，想自动加则保留
                git add . 2>&1 | Out-Null

                # 检查是否有可提交的变更
                $changes = git status --porcelain
                if ([string]::IsNullOrWhiteSpace($changes)) {
                    Write-Info "无可用的提交变更！" $COLOR_RED; Start-Sleep 2; continue
                }

                # 步骤4：提交
                $msg = Get-CleanInput "Commit 备注"
                if ($msg) {
                    git commit -m "$msg"
                    # 推送（这里才用到 $GITEE_REMOTE）
                    if ((Get-CleanInput "推送? (Y/N)") -eq "Y") { 
                        git push $GITEE_REMOTE (git branch --show-current).Trim() 
                    }
                }
                $selectedFiles = @()
                continue
            }

            # 4. 目录导航
            if ($cmd -match "^cd\s+(\d+)$") {
                $num = [int]$matches[1]
                if ($indexMap.ContainsKey($num) -and $indexMap[$num].PSIsContainer) {
                    $currentBrowsePath = $indexMap[$num].FullName
                } else { Write-Info "无效目录。" $COLOR_RED; Start-Sleep 1 }
                continue
            }

            # 5. 上一级
            if ($cmd -eq "0") {
                if ($currentBrowsePath -ne $localRoot) { $currentBrowsePath = Split-Path $currentBrowsePath -Parent }
                continue
            }

            # 6. 选择/取消
            if ($cmd -match "^\d+$") {
                $num = [int]$cmd
                if ($indexMap.ContainsKey($num)) {
                    $target = $indexMap[$num].FullName
                    if ($selectedFiles -contains $target) {
                        $selectedFiles = $selectedFiles | Where-Object { $_ -ne $target }
                    } else { $selectedFiles += $target }
                }
                continue
            }
        }
    }
}

# --- 功能 4: 帮助 ---
function Module-Help {
    Show-Header
    Write-Info "[功能4] 帮助" $COLOR_YELLOW
    Write-Info "cd [数字] : 进入文件夹" $COLOR_WHITE
    Write-Info "[数字]    : 标记为待提交 (git add)" $COLOR_WHITE
    Write-Info "0         : 返回上一级目录" $COLOR_WHITE
    Write-Info "B         : 返回到分支选择界面 (Back)" $COLOR_WHITE
    Write-Info "Q         : 清理暂存区并安全退出 (Quit)" $COLOR_WHITE
    Wait-Input
}

# ==========================================
# 主程序
# ==========================================

while ($true) {
    Show-Header
    Write-Info "1. 分支查看与本地分支删除" $COLOR_WHITE
    Write-Info "2. 分支选择与仓库文件管理 (核心)" $COLOR_WHITE
    Write-Info "3. 清理暂存区" $COLOR_WHITE
    Write-Info "4. 帮助" $COLOR_WHITE
    
    $choice = Get-CleanInput "`n请选择 (1-4, Q退出)"
    
    if ($choice -eq "Q") { Safe-Exit } # 主菜单也响应 Q

    switch ($choice) {
        "1" { Module-ManageBranches }
        "2" { Module-FileAndBranch }
        "3" { Module-CleanStaging; Wait-Input }
        "4" { Module-Help }
    }
}