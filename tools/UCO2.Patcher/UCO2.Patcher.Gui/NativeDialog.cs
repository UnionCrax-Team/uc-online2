using System;
using System.Runtime.InteropServices;
using System.Windows;
using System.Windows.Interop;

namespace UCO2.Patcher.Gui;

/// <summary>
/// Modern native Task Dialogs (comctl32 v6) in place of the dated Win32
/// MessageBox. A WPF app already carries the comctl6 dependency in its manifest,
/// so the simple TaskDialog export renders the Windows 10/11 styled dialog
/// directly. Falls back to MessageBox if the call fails for any reason.
/// </summary>
public static class NativeDialog
{
    private const int TDCBF_OK  = 0x0001;
    private const int TDCBF_YES = 0x0002;
    private const int TDCBF_NO  = 0x0004;

    // MAKEINTRESOURCE(-1..-3): the built-in task-dialog icons.
    private static readonly IntPtr TD_WARNING_ICON     = 0xFFFF;
    private static readonly IntPtr TD_ERROR_ICON       = 0xFFFE;
    private static readonly IntPtr TD_INFORMATION_ICON = 0xFFFD;

    private const int IDYES = 6;

    [DllImport("comctl32.dll", CharSet = CharSet.Unicode, EntryPoint = "TaskDialog", ExactSpelling = true)]
    private static extern int TaskDialog(
        IntPtr hwndParent, IntPtr hInstance,
        string pszWindowTitle, string pszMainInstruction, string pszContent,
        int dwCommonButtons, IntPtr pszIcon, out int pnButton);

    public static void Info(Window? owner, string heading, string message) =>
        Show(owner, heading, message, TDCBF_OK, TD_INFORMATION_ICON, MessageBoxButton.OK, MessageBoxImage.Information);

    public static void Warn(Window? owner, string heading, string message) =>
        Show(owner, heading, message, TDCBF_OK, TD_WARNING_ICON, MessageBoxButton.OK, MessageBoxImage.Warning);

    public static void Error(Window? owner, string heading, string message) =>
        Show(owner, heading, message, TDCBF_OK, TD_ERROR_ICON, MessageBoxButton.OK, MessageBoxImage.Error);

    /// <summary>Yes/No confirmation. Pass warning:true for destructive actions.</summary>
    public static bool Confirm(Window? owner, string heading, string message, bool warning = false) =>
        Show(owner, heading, message, TDCBF_YES | TDCBF_NO,
             warning ? TD_WARNING_ICON : TD_INFORMATION_ICON,
             MessageBoxButton.YesNo, warning ? MessageBoxImage.Warning : MessageBoxImage.Question) == IDYES;

    private static int Show(Window? owner, string heading, string message, int buttons, IntPtr icon,
                            MessageBoxButton fallbackButtons, MessageBoxImage fallbackImage)
    {
        IntPtr hwnd = owner is not null ? new WindowInteropHelper(owner).Handle : IntPtr.Zero;
        try
        {
            if (TaskDialog(hwnd, IntPtr.Zero, "UCOnline2", heading, message, buttons, icon, out int button) == 0)
                return button;
        }
        catch { /* fall through to MessageBox */ }

        MessageBoxResult result = owner is not null
            ? MessageBox.Show(owner, message, heading, fallbackButtons, fallbackImage)
            : MessageBox.Show(message, heading, fallbackButtons, fallbackImage);
        return result == MessageBoxResult.Yes ? IDYES : 0;
    }
}
