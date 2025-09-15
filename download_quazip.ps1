# QuaZip库下载脚本
Write-Host "正在下载QuaZip库..." -ForegroundColor Green

# 创建目录
if (!(Test-Path "libs")) {
    New-Item -ItemType Directory -Path "libs"
}
if (!(Test-Path "libs\include")) {
    New-Item -ItemType Directory -Path "libs\include"
}
if (!(Test-Path "libs\lib")) {
    New-Item -ItemType Directory -Path "libs\lib"
}

# QuaZip GitHub仓库信息
$repoUrl = "https://api.github.com/repos/stachenov/quazip/releases/latest"

try {
    Write-Host "获取最新版本信息..." -ForegroundColor Yellow
    $response = Invoke-RestMethod -Uri $repoUrl -Headers @{"Accept"="application/vnd.github.v3+json"}
    $latestVersion = $response.tag_name
    Write-Host "最新版本: $latestVersion" -ForegroundColor Cyan
    
    # 查找Windows预编译版本
    $windowsAsset = $response.assets | Where-Object { $_.name -like "*windows*" -or $_.name -like "*win32*" -or $_.name -like "*.zip" }
    
    if ($windowsAsset) {
        Write-Host "找到Windows版本: $($windowsAsset.name)" -ForegroundColor Green
        Write-Host "下载链接: $($windowsAsset.browser_download_url)" -ForegroundColor Cyan
        
        # 下载文件
        $downloadPath = "libs\quazip.zip"
        Write-Host "正在下载到: $downloadPath" -ForegroundColor Yellow
        
        # 使用WebClient下载
        $webClient = New-Object System.Net.WebClient
        $webClient.DownloadFile($windowsAsset.browser_download_url, $downloadPath)
        
        Write-Host "下载完成！" -ForegroundColor Green
        
        # 解压文件
        Write-Host "正在解压..." -ForegroundColor Yellow
        Expand-Archive -Path $downloadPath -DestinationPath "libs" -Force
        
        # 清理zip文件
        Remove-Item $downloadPath -Force
        
        Write-Host "QuaZip库安装完成！" -ForegroundColor Green
        
    } else {
        Write-Host "未找到Windows预编译版本，将使用源码编译方式..." -ForegroundColor Yellow
        Write-Host "请手动下载源码并编译，或使用vcpkg安装。" -ForegroundColor Red
    }
    
} catch {
    Write-Host "网络请求失败: $($_.Exception.Message)" -ForegroundColor Red
    Write-Host "请检查网络连接或手动下载QuaZip库。" -ForegroundColor Yellow
}

Write-Host "`n安装说明：" -ForegroundColor Cyan
Write-Host "1. 如果下载成功，请检查libs目录下的文件结构" -ForegroundColor White
Write-Host "2. 确保包含以下文件：" -ForegroundColor White
Write-Host "   - libs/include/quazip/ 目录" -ForegroundColor White
Write-Host "   - libs/lib/ 目录下的库文件" -ForegroundColor White
Write-Host "3. 如果文件结构不正确，请手动调整" -ForegroundColor White

Read-Host "按回车键继续"
