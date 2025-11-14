@echo off
echo ============================================================
echo Restarting Uber Mini Server with New Complex Graph
echo ============================================================
echo.
echo Stopping any existing server on port 3000...

REM Kill any process using port 3000
for /f "tokens=5" %%a in ('netstat -ano ^| findstr :3000 ^| findstr LISTENING') do taskkill /F /PID %%a 2>nul

timeout /t 2 /nobreak >nul

echo.
echo Starting server with complex road network...
echo.
echo New features:
echo - 50 nodes (increased from 30)
echo - Highways, arterial roads, local streets
echo - Ring roads around city center
echo - Bridges and shortcuts
echo - Realistic location names by district
echo - 12 drivers spread across the city
echo.
echo ============================================================
echo.

node backend/server.js
