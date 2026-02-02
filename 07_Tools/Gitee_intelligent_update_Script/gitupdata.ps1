# GiteeSync_Smart_V4.1.ps1
$OutputEncoding = [System.Text.Encoding]::UTF8
[Console]::OutputEncoding = [System.Text.Encoding]::UTF8

# ================= 配置区域 =================
$RepoPath = "E:\Git_Repository\Watch_project\EmbeddedProject_Folder"
$GiteeUrl = "https://gitee.com/zhang___qi/watch_project.git"
$MainBranch = "master"
# ===========================================

if (Test-Path $RepoPath) { Set-Location $RepoPath } else { Write-Host "❌ 找不到目录"; exit }

if (-not (Test-Path ".git")) { 
    git init
    git checkout -b $MainBranch 2>$null
    Write-Host "✨ 已初始化本地仓库并创建 $MainBranch 分支" -ForegroundColor Cyan
}

git config --local core.safecrlf false
git config --local core.quotePath false

$curRemote = git remote get-url origin 2>$null
if ($curRemote -ne $GiteeUrl) { 
    if($curRemote){ git remote set-url origin $GiteeUrl } else { git remote add origin $GiteeUrl } 
}

# --- 核心函数：智能选择文件 ---
function Select-Files-Smart {
    Write-Host "`n--- 📦 智能选择模式 ---" -ForegroundColor Cyan
    while ($true) {
        $changes = git status --porcelain -u
        if ([string]::IsNullOrWhiteSpace($changes)) { Write-Host "✨ 暂存区已就绪。" -ForegroundColor Green; break }

        $lines = $changes -split "`n"
        $groupedItems = @{}
        $displayList = @()

        foreach ($line in $lines) {
            if (-not [string]::IsNullOrWhiteSpace($line)) {
                $status = $line.Substring(0, 2)
                $rawPath = $line.Substring(3).Trim('"')
                $parts = $rawPath -split '/'
                if ($parts.Count -gt 1) { $key = $parts[0] + "/"; $type = "📂 工程/模块"; $isFolder = $true } 
                else { $key = $rawPath; $type = "📄 根文件"; $isFolder = $false }
                
                $s2 = $status[1]; $isUnstaged = ($s2 -ne ' ')
                if (-not $groupedItems.ContainsKey($key)) {
                    $obj = [PSCustomObject]@{ Path = $key; Type = $type; IsFolder = $isFolder; UnstagedCount = 0; TotalCount = 0 }
                    $groupedItems[$key] = $obj; $displayList += $obj
                }
                $groupedItems[$key].TotalCount++; if ($isUnstaged) { $groupedItems[$key].UnstagedCount++ }
            }
        }

        Write-Host "当前变更列表：" -ForegroundColor Yellow
        for ($i = 0; $i -lt $displayList.Count; $i++) {
            $item = $displayList[$i]
            Write-Host "[$($i+1)] " -NoNewline -ForegroundColor Gray
            if ($item.UnstagedCount -gt 0) { Write-Host "🟥 [待暂存] " -NoNewline -ForegroundColor Red } 
            else { Write-Host "✅ [已暂存] " -NoNewline -ForegroundColor Green }
            Write-Host "$($item.Type) $($item.Path) " -NoNewline -ForegroundColor White
            if ($item.IsFolder) { Write-Host "($($item.TotalCount) 个文件)" -ForegroundColor DarkGray } else { Write-Host "" }
        }

        $selection = Read-Host "`n操作: 输入序号添加 | 'a' 全选 | 'c' 清空暂存 | 回车 确认进入下一步"
        if ([string]::IsNullOrWhiteSpace($selection)) { break }
        if ($selection -eq 'a') { git add .; break }
        if ($selection -eq 'c') { git reset; continue }
        
        $indices = $selection -split '\s+'
        foreach ($idx in $indices) {
            if ($idx -match '^\d+$' -and [int]$idx -ge 1 -and [int]$idx -le $displayList.Count) {
                $target = $displayList[[int]$idx - 1]
                git add "$($target.Path)"
                Write-Host "已暂存: $($target.Path)" -ForegroundColor DarkGray
            }
        }
    }
}

function Ask-User($q) { do{$i=Read-Host "$q (Y/N)";if($i -match '^[Yy]'){return $true};if($i -match '^[Nn]'){return $false}}while($true) }

# ================= 主流程 =================

Write-Host "`n==========================================" -ForegroundColor Cyan
Write-Host "      Gitee 嵌入式项目流工具 V4.1         " -ForegroundColor Cyan
Write-Host "==========================================" -ForegroundColor Cyan

# Step 1: 模式选择
Write-Host "请选择工作模式：" -ForegroundColor Yellow
Write-Host "1. [结构同步] 维护 $MainBranch 分支 (更新 00-07 基础结构)"
Write-Host "2. [功能开发] 基于 $MainBranch 创建/切换功能分支 (开发 03_Firmware)"
Write-Host "3. [合并主干] 将开发分支代码 合并回 $MainBranch (稳定版本推送)"
$mode = Read-Host ">>> 请选择 (1/2/3)"

$currentBranch = git branch --show-current

if ($mode -eq '1') {
    if ($currentBranch -ne $MainBranch) { 
        Write-Host "正在切换到 $MainBranch..."
        git checkout $MainBranch 2>$null
        if ($LASTEXITCODE -ne 0) { git checkout -b $MainBranch } 
    }
} 
elseif ($mode -eq '2') {
    if ($currentBranch -eq $MainBranch) {
        $featureName = Read-Host "请输入功能分支名称 (例如: dev-watch-v1)"
        if ([string]::IsNullOrWhiteSpace($featureName)) { Write-Host "❌ 分支名不能为空"; exit }
        git checkout -b $featureName
        $currentBranch = $featureName
    }
}
elseif ($mode -eq '3') {
    if ($currentBranch -eq $MainBranch) { Write-Host "❌ 当前已在 $MainBranch，请切换到要合并的功能分支"; exit }
    if (Ask-User "确定要将 [$currentBranch] 的代码合并到 $MainBranch 吗？") {
        $devBranch = $currentBranch
        git add .
        git commit -m "pre-merge commit" 2>$null
        git checkout $MainBranch
        git pull origin $MainBranch
        git merge $devBranch
        if ($LASTEXITCODE -eq 0) {
            git push origin $MainBranch
            if (Ask-User "是否删除功能分支 $devBranch ?") { git branch -d $devBranch }
            Write-Host "🎉 合并完成！" -ForegroundColor Green
        } else {
            Write-Host "❌ 合并存在冲突，请手动解决后提交。" -ForegroundColor Red
        }
    }
    exit
}

# Step 2: 拉取更新
$targetBranch = git branch --show-current
Write-Host "`n[拉取] 检查远程 $targetBranch 更新..." -ForegroundColor Yellow
git fetch origin $targetBranch 2>$null
$statusSB = git status -sb
if ($statusSB -match "behind") {
    if (Ask-User "远程有新内容，是否拉取合并？") { git pull origin $targetBranch }
}

# Step 3: 选择并暂存文件
Select-Files-Smart

# Step 4: 提交
$staged = git diff --name-only --cached
if ($staged) {
    Write-Host "`n[提交] 正在准备发布..." -ForegroundColor Yellow
    if ($staged -is [array]) { 
        if ($staged.Count -gt 10) { Write-Host "📦 已选择 $($staged.Count) 个文件" -ForegroundColor Gray }
        else { Write-Host ($staged -join "`n") -ForegroundColor Gray }
    } else { Write-Host $staged -ForegroundColor Gray }

    $msg = Read-Host "`n输入提交备注 (回车使用默认日期备注)"
    if (-not $msg) { $msg = "Update ${targetBranch}: " + (Get-Date -Format "yyyy-MM-dd HH:mm") }
    git commit -m "$msg"
    
    # Step 5: 推送
    if (Ask-User "确认推送至远程 Gitee [$targetBranch] ？") {
        git push origin $targetBranch
        if ($LASTEXITCODE -ne 0) { git push --set-upstream origin $targetBranch }
        Write-Host "🎉 同步完成！" -ForegroundColor Green
    }
} else {
    Write-Host "⚠️  未发现待提交内容，操作结束。" -ForegroundColor Yellow
}

Write-Host "`n按任意键退出..."
$null = $Host.UI.RawUI.ReadKey("NoEcho,IncludeKeyDown")