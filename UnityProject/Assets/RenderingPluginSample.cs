using UnityEngine;
using System;
using System.Collections;
using System.Runtime.InteropServices;
using UnityEngine.UI;

public class RenderingPluginSample : MonoBehaviour
{
    [SerializeField] private InputField _vertex;
    [SerializeField] private InputField _fragment;
    [SerializeField] private Button _compileButton;

    [DllImport("RenderingPlugin")]
    private static extern IntPtr GetRenderEventFunc();

    [DllImport("RenderingPlugin")]
    private static extern void SetShaders(string vertexShader, string fragmentShader);

    private void OnEnable() => _compileButton.onClick.AddListener(CompileShaders);

    private void OnDisable() => _compileButton.onClick.RemoveListener(CompileShaders);

    private void Start()
    {
        StartCoroutine(CallPluginAtEndOfFrames());
    }

    private void CompileShaders()
    {
        SetShaders(_vertex.text, _fragment.text);
        GL.IssuePluginEvent(GetRenderEventFunc(), 2);
    }

    private IEnumerator CallPluginAtEndOfFrames()
    {
        while (true)
        {
            yield return new WaitForEndOfFrame();
            GL.IssuePluginEvent(GetRenderEventFunc(), 1);
        }
    }
}
