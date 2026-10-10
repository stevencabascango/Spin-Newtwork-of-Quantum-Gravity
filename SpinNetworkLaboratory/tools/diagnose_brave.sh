#!/usr/bin/env bash
# Run outside Codex in a regular WSL terminal.
set -euo pipefail
windows_powershell='/mnt/c/Windows/System32/WindowsPowerShell/v1.0/powershell.exe'
payload=$(python3 - <<'PY'
import base64
script=r"""$ErrorActionPreference = 'Continue'
$ProgressPreference = 'SilentlyContinue'
[Console]::OutputEncoding = New-Object System.Text.UTF8Encoding($false)
$OutputEncoding = [Console]::OutputEncoding
Set-Location -LiteralPath $env:TEMP
Write-Output '--- Windows Node and npm ---'
Get-Command node.exe,npx.cmd | Select-Object Name,Source | Format-Table -AutoSize
Write-Output '--- Playwright MCP startup check ---'
& npx.cmd '-y' '@playwright/mcp@latest' '--help'
exit $LASTEXITCODE
"""
print(base64.b64encode(script.encode('utf-16le')).decode('ascii'))
PY
)
cd /mnt/c
"$windows_powershell" -NoLogo -NoProfile -NonInteractive -OutputFormat Text -EncodedCommand "$payload"
