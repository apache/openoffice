param(
    [string]$FilePath
)

$hash = (Get-FileHash $FilePath -Algorithm SHA256).Hash
$bytes = [byte[]] -split ($hash -replace '..', '0x$& ')
$base64 = [Convert]::ToBase64String($bytes)
Write-Output "sha256-$base64"